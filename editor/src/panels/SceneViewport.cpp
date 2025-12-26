#include "panels/SceneViewport.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/TextureSource.h"
#include "origo/scene/Transform.h"
#include "state/EditorRuntimeState.h"

namespace OrigoEditor {

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
	ImVec2 viewportSize {
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

	drawToolButton("##Move", m_MoveIcon, ImGuizmo::TRANSLATE);
	ImGui::SameLine();
	drawToolButton("##Rotate", m_RotateIcon, ImGuizmo::ROTATE);
	ImGui::SameLine();
	drawToolButton("##Scale", m_ScaleIcon, ImGuizmo::SCALE);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	if (ImGui::Button(m_GizmoMode == ImGuizmo::LOCAL ? "Local" : "World"))
		m_GizmoMode = (m_GizmoMode == ImGuizmo::LOCAL)
		    ? ImGuizmo::WORLD
		    : ImGuizmo::LOCAL;

	ImGui::EndGroup();
	ImGui::PopStyleVar(2);

	if (m_Context.SelectedEntity.has_value() && m_Context.RuntimeState == EditorRuntimeState::Editing) {
		auto& entity = m_Context.SelectedEntity.value();
		auto transform = m_Context.EditorScene->GetNativeComponent<Origo::Transform>(entity);

		glm::mat4 model = transform->GetModelMatrix();
		glm::mat4 view = m_Camera.GetCamera().GetView();
		glm::mat4 proj = m_Camera.GetCamera().GetProjection();

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

		m_Context.EditorViewportCamera.OnMouseDelta(smoothDeltaGlm);
	}
}

}
