#include "panels/SceneViewport.h"

#include "origo/assets/Texture2D.h"
#include "origo/assets/TextureSource.h"

#include "origo/components/Transform.h"
#include "origo/core/PathContext.h"

#include "state/EditorViewMode.h"

using namespace Origo;

namespace OrigoEditor {

static void view_mode_toggle(EditorViewMode& mode) {
	ImDrawList* draw = ImGui::GetWindowDrawList();
	ImVec2 pos = ImGui::GetCursorScreenPos();

	const float height = 28.0f;
	const float radius = 7.0f;
	const float padding = 3.0f;

	const char* labels[] = { "Scene", "Run" };
	const EditorViewMode modes[] = {
		EditorViewMode::Editor,
		EditorViewMode::Game
	};

	float widths[2];
	for (int i = 0; i < 2; i++)
		widths[i] = ImGui::CalcTextSize(labels[i]).x + 20.0f;

	float total_width = widths[0] + widths[1] + padding * 2;

	ImVec2 size(total_width, height);
	ImGui::InvisibleButton("##viewmode", size);

	draw->AddRectFilled(
	    pos,
	    pos + size,
	    IM_COL32(40, 40, 40, 255),
	    radius);

	float x = pos.x + padding;

	for (int i = 0; i < 2; i++) {
		bool active = (mode == modes[i]);

		ImVec2 seg_min(x, pos.y + padding);
		ImVec2 seg_max(x + widths[i], pos.y + height - padding);

		if (active) {
			draw->AddRectFilled(
			    seg_min,
			    seg_max,
			    IM_COL32(90, 90, 90, 255),
			    radius);
		}

		ImVec2 text_size = ImGui::CalcTextSize(labels[i]);
		ImVec2 text_pos(
		    seg_min.x + (widths[i] - text_size.x) * 0.5f,
		    pos.y + (height - text_size.y) * 0.5f);

		draw->AddText(
		    text_pos,
		    active ? IM_COL32(255, 255, 255, 255)
		           : IM_COL32(160, 160, 160, 255),
		    labels[i]);

		if (ImGui::IsMouseHoveringRect(seg_min, seg_max) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			mode = modes[i];
		}

		x += widths[i];
	}
}

static ImTextureID to_im_texture_id(const Ref<Texture2D>& tex) {
	return (ImTextureID)(intptr_t)tex->get_renderer_id();
}

static Ref<Texture2D> load_svg_texture(const std::string& path, int size = 18) {
	auto texture = make_ref<Texture2D>(TextureType::UI);
	texture->set_source(make_scope<TextureSourceSVG>(
	    PathContextService::get_instance().editor().resolve(path).string(),
	    size,
	    size));
	texture->load();

	return texture;
}

void SceneViewport::on_im_gui_render() {
	constexpr float sensitivity = 0.3f;
	constexpr float max_delta = 50.0f;
	constexpr float smooth_alpha = 0.2f;

	if (!m_icons_loaded) {
		m_move_icon = load_svg_texture("icons/ToolMove.svg", 18);
		m_rotate_icon = load_svg_texture("icons/ToolRotate.svg", 18);
		m_scale_icon = load_svg_texture("icons/ToolScale.svg", 18);
		m_icons_loaded = true;
	}

	auto& active_scene { m_context.ActiveScene };
	ImGuizmo::BeginFrame();

	ImVec2 size = ImGui::GetContentRegionAvail();
	int w = (int)size.x;
	int h = (int)size.y;

	if (w <= 0 || h <= 0) {
		ImGui::Dummy(size);
		return;
	}

	static int last_w = 0, last_h = 0;
	bool resized = false;

	if (w != last_w || h != last_h) {
		m_context.RenderBuffer.resize(w, h);
		m_context.ResolveBuffer.resize(w, h);
		last_w = w;
		last_h = h;
		resized = true;
	}

	ImGui::Image(
	    (ImTextureID)(uintptr_t)m_context.ResolveBuffer.get_color_attachment(0),
	    size,
	    ImVec2(0, 1),
	    ImVec2(1, 0));

	ImVec2 viewport_min = ImGui::GetItemRectMin();
	ImVec2 viewport_max = ImGui::GetItemRectMax();
	ImVec2 viewport_size = {
		viewport_max.x - viewport_min.x,
		viewport_max.y - viewport_min.y
	};

	ImGui::SetCursorScreenPos({ viewport_min.x + 8.0f, viewport_min.y + 8.0f });

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6, 4));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));
	ImGui::BeginGroup();

	const ImVec4 inactive_tint(0.85f, 0.85f, 0.85f, 1.0f);
	const ImVec4 active_tint(0.30f, 0.60f, 1.00f, 1.0f);

	auto draw_tool_button = [&](const char* id,
	                            const Ref<Texture2D>& icon,
	                            ImGuizmo::OPERATION op) {
		ImVec4 tint = (m_gizmo_operation == op) ? active_tint : inactive_tint;

		if (ImGui::ImageButton(
		        id,
		        to_im_texture_id(icon),
		        ImVec2(18, 18),
		        ImVec2(0, 1),
		        ImVec2(1, 0),
		        ImVec4(0, 0, 0, 0),
		        tint)) {
			m_gizmo_operation = op;
		}
	};

	const bool editing_view = (m_context.ViewMode == EditorViewMode::Editor);

	if (editing_view) {
		draw_tool_button("##Move", m_move_icon, ImGuizmo::TRANSLATE);
		ImGui::SameLine();
		draw_tool_button("##Rotate", m_rotate_icon, ImGuizmo::ROTATE);
		ImGui::SameLine();
		draw_tool_button("##Scale", m_scale_icon, ImGuizmo::SCALE);

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		if (ImGui::Button(m_gizmo_mode == ImGuizmo::LOCAL ? "Local" : "World")) {
			m_gizmo_mode = (m_gizmo_mode == ImGuizmo::LOCAL)
			    ? ImGuizmo::WORLD
			    : ImGuizmo::LOCAL;
		}

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
	}

	view_mode_toggle(m_context.ViewMode);

	ImGui::EndGroup();
	ImGui::PopStyleVar(2);

	if (editing_view && m_context.get_selected_entity().has_value()) {
		auto entity = *m_context.get_selected_entity();
		auto transform = active_scene->get_native_component<TransformComponent>(entity);

		glm::mat4 model = transform->get_model_matrix();
		auto render_view = m_context.ViewportController.get_and_update_active_render_view();
		glm::mat4 view = render_view.View;
		glm::mat4 proj = render_view.Projection;

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(
		    viewport_min.x,
		    viewport_min.y,
		    viewport_size.x,
		    viewport_size.y);

		ImGuizmo::Manipulate(
		    glm::value_ptr(view),
		    glm::value_ptr(proj),
		    m_gizmo_operation,
		    m_gizmo_mode,
		    glm::value_ptr(model));

		if (ImGuizmo::IsUsing())
			transform->set_from_matrix(model);
	}

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 mouse = ImGui::GetMousePos();

	bool hovered = mouse.x >= viewport_min.x && mouse.y >= viewport_min.y && mouse.x <= viewport_max.x && mouse.y <= viewport_max.y;

	static bool dragging = false;
	static ImVec2 smooth_delta { 0.0f, 0.0f };

	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		dragging = true;

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		dragging = false;

	if (ImGuizmo::IsUsing())
		dragging = false;

	if (dragging && !resized && !ImGuizmo::IsUsing()) {
		ImVec2 delta = io.MouseDelta;

		delta.x = std::clamp(delta.x, -max_delta, max_delta);
		delta.y = std::clamp(delta.y, -max_delta, max_delta);

		smooth_delta.x = smooth_delta.x * (1.0f - smooth_alpha) + delta.x * smooth_alpha;
		smooth_delta.y = smooth_delta.y * (1.0f - smooth_alpha) + delta.y * smooth_alpha;

		glm::vec2 smooth_delta_glm {
			(smooth_delta * sensitivity).x,
			(smooth_delta * sensitivity).y
		};

		if (m_context.ViewMode == EditorViewMode::Editor)
			m_camera.on_mouse_delta(smooth_delta_glm);
	}
}
}
