#include "panels/SceneViewport.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/TextureSource.h"
#include "origo/scene/Transform.h"
#include "state/EditorViewMode.h"

namespace OrigoEditor {

static void ViewModeToggle(EditorViewMode& mode) {
	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImVec2 pos = ImGui::GetCursorScreenPos();

	const float height = 28.0f;
	const float radius = 7.0f;
	const float padding = 3.0f;

	const char* labels[] = { "Scene", "Run" };
	const EditorViewMode modes[] = {
		EditorViewMode::Editor,
		EditorViewMode::Run
	};

	float widths[2];
	for (int i = 0; i < 2; i++)
		widths[i] = ImGui::CalcTextSize(labels[i]).x + 20.0f;

	float totalWidth = widths[0] + widths[1] + padding * 2;

	ImVec2 size(totalWidth, height);
	ImGui::InvisibleButton("##viewmode", size);

	draw->AddRectFilled(
	    pos,
	    pos + size,
	    IM_COL32(40, 40, 40, 255),
	    radius);

	float x = pos.x + padding;

	for (int i = 0; i < 2; i++) {
		bool active = (mode == modes[i]);

		ImVec2 segMin(x, pos.y + padding);
		ImVec2 segMax(x + widths[i], pos.y + height - padding);

		if (active) {
			draw->AddRectFilled(
			    segMin,
			    segMax,
			    IM_COL32(90, 90, 90, 255),
			    radius);
		}

		ImVec2 textSize = ImGui::CalcTextSize(labels[i]);
		ImVec2 textPos(
		    segMin.x + (widths[i] - textSize.x) * 0.5f,
		    pos.y + (height - textSize.y) * 0.5f);

		draw->AddText(
		    textPos,
		    active ? IM_COL32(255, 255, 255, 255)
		           : IM_COL32(160, 160, 160, 255),
		    labels[i]);

		if (ImGui::IsMouseHoveringRect(segMin, segMax) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			mode = modes[i];
		}

		x += widths[i];
	}
}

static ImTextureID ToImTextureID(const Origo::Ref<Origo::Texture2D>& tex) {
	return (ImTextureID)(intptr_t)tex->GetRendererID();
}

static Origo::Ref<Origo::Texture2D> LoadSVGTexture(const std::string& path, int size = 18) {
	auto texture = Origo::MakeRef<Origo::Texture2D>(Origo::TextureType::UI);
	texture->SetSource(Origo::MakeScope<Origo::TextureSourceSVG>(path, size, size));
	texture->Load();

	return texture;
}

void SceneViewport::OnImGuiRender() {
	constexpr float sensitivity = 0.3f;
	constexpr float maxDelta = 50.0f;
	constexpr float smoothAlpha = 0.2f;

	if (!m_IconsLoaded) {
		m_MoveIcon = LoadSVGTexture("icons/ToolMove.svg", 18);
		m_RotateIcon = LoadSVGTexture("icons/ToolRotate.svg", 18);
		m_ScaleIcon = LoadSVGTexture("icons/ToolScale.svg", 18);
		m_IconsLoaded = true;
	}

	auto& activeScene { m_Context.ActiveScene };
	ImGuizmo::BeginFrame();

	ImVec2 size = ImGui::GetContentRegionAvail();
	int w = (int)size.x;
	int h = (int)size.y;

	if (w <= 0 || h <= 0) {
		ImGui::Dummy(size);
		return;
	}

	static int lastW = 0, lastH = 0;
	bool resized = false;

	if (w != lastW || h != lastH) {
		m_Context.RenderBuffer.Resize(w, h);
		m_Context.ResolveBuffer.Resize(w, h);
		lastW = w;
		lastH = h;
		resized = true;
	}

	ImGui::Image(
	    (ImTextureID)(uintptr_t)m_Context.ResolveBuffer.GetColorAttachment(0),
	    size,
	    ImVec2(0, 1),
	    ImVec2(1, 0));

	ImVec2 viewportMin = ImGui::GetItemRectMin();
	ImVec2 viewportMax = ImGui::GetItemRectMax();
	ImVec2 viewportSize = {
		viewportMax.x - viewportMin.x,
		viewportMax.y - viewportMin.y
	};

	ImGui::SetCursorScreenPos({ viewportMin.x + 8.0f, viewportMin.y + 8.0f });

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));
	ImGui::BeginGroup();

	const ImVec4 inactiveTint(0.85f, 0.85f, 0.85f, 1.0f);
	const ImVec4 activeTint(0.30f, 0.60f, 1.00f, 1.0f);

	auto drawToolButton = [&](const char* id,
	                          const Origo::Ref<Origo::Texture2D>& icon,
	                          ImGuizmo::OPERATION op) {
		ImVec4 tint = (m_GizmoOperation == op) ? activeTint : inactiveTint;

		if (ImGui::ImageButton(
		        id,
		        ToImTextureID(icon),
		        ImVec2(18, 18),
		        ImVec2(0, 1),
		        ImVec2(1, 0),
		        ImVec4(0, 0, 0, 0),
		        tint)) {
			m_GizmoOperation = op;
		}
	};

	const bool editingView = (m_Context.ViewMode == EditorViewMode::Editor);

	if (editingView) {
		drawToolButton("##Move", m_MoveIcon, ImGuizmo::TRANSLATE);
		ImGui::SameLine();
		drawToolButton("##Rotate", m_RotateIcon, ImGuizmo::ROTATE);
		ImGui::SameLine();
		drawToolButton("##Scale", m_ScaleIcon, ImGuizmo::SCALE);

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		if (ImGui::Button(m_GizmoMode == ImGuizmo::LOCAL ? "Local" : "World")) {
			m_GizmoMode = (m_GizmoMode == ImGuizmo::LOCAL)
			    ? ImGuizmo::WORLD
			    : ImGuizmo::LOCAL;
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
	}

	ViewModeToggle(m_Context.ViewMode);

	ImGui::EndGroup();
	ImGui::PopStyleVar(2);

	if (editingView && m_Context.SelectedEntity.has_value()) {
		auto& entity = m_Context.SelectedEntity.value();
		auto transform = activeScene->GetNativeComponent<Origo::Transform>(entity);

		glm::mat4 model = transform->GetModelMatrix();
		glm::mat4 view = m_Context.ViewportController.GetActiveRenderView().View;
		glm::mat4 proj = m_Context.ViewportController.GetActiveRenderView().Projection;

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
		    viewportMin.x,
		    viewportMin.y,
		    viewportSize.x,
		    viewportSize.y);

		ImGuizmo::Manipulate(
		    glm::value_ptr(view),
		    glm::value_ptr(proj),
		    m_GizmoOperation,
		    m_GizmoMode,
		    glm::value_ptr(model));

		if (ImGuizmo::IsUsing())
			transform->SetFromMatrix(model);
	}

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 mouse = ImGui::GetMousePos();

	bool hovered = mouse.x >= viewportMin.x && mouse.y >= viewportMin.y && mouse.x <= viewportMax.x && mouse.y <= viewportMax.y;

	static bool dragging = false;
	static ImVec2 smoothDelta { 0.0f, 0.0f };

	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		dragging = true;

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		dragging = false;

	if (ImGuizmo::IsUsing())
		dragging = false;

	if (dragging && !resized && !ImGuizmo::IsUsing()) {
		ImVec2 delta = io.MouseDelta;

		delta.x = std::clamp(delta.x, -maxDelta, maxDelta);
		delta.y = std::clamp(delta.y, -maxDelta, maxDelta);

		smoothDelta.x = smoothDelta.x * (1.0f - smoothAlpha) + delta.x * smoothAlpha;
		smoothDelta.y = smoothDelta.y * (1.0f - smoothAlpha) + delta.y * smoothAlpha;

		glm::vec2 smoothDeltaGlm {
			(smoothDelta * sensitivity).x,
			(smoothDelta * sensitivity).y
		};

		if (m_Context.ViewMode == EditorViewMode::Editor)
			m_Camera.OnMouseDelta(smoothDeltaGlm);
	}
}
}
