#include "panels/SceneViewport.h"

#include <algorithm>

#include "origo/assets/Texture2D.h"
#include "origo/assets/TextureSource.h"

#include "origo/components/Transform.h"
#include "origo/core/PathContext.h"

#include "state/EditorViewMode.h"

using namespace Origo;

namespace OrigoEditor {
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
		m_snap_grid_icon = load_svg_texture("icons/SnapGrid.svg", 18);
		m_icons_loaded = true;
	}

	auto* active_scene = m_context.get_viewport_scene();
	ImGuizmo::BeginFrame();

	ImVec2 size = ImGui::GetContentRegionAvail();
	int w = (int)size.x;
	int h = (int)size.y;

	if (w <= 0 || h <= 0) {
		m_context.set_viewport_visible(m_mode, false);
		ImGui::Dummy(size);
		return;
	}

	m_context.set_viewport_visible(m_mode, true);

	bool resized = false;
	auto& render_buffer = m_context.get_render_buffer(m_mode);
	auto& resolve_buffer = m_context.get_resolve_buffer(m_mode);

	if (w != m_last_width || h != m_last_height) {
		render_buffer.resize(w, h);
		resolve_buffer.resize(w, h);
		m_last_width = w;
		m_last_height = h;
		resized = true;
	}

	ImGui::Image(
	    (ImTextureID)(uintptr_t)resolve_buffer.get_color_attachment(0),
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

	auto draw_toggle_button = [&](const char* id,
	                              const Ref<Texture2D>& icon,
	                              bool active) {
		ImVec4 tint = active ? active_tint : inactive_tint;
		return ImGui::ImageButton(
		    id,
		    to_im_texture_id(icon),
		    ImVec2(18, 18),
		    ImVec2(0, 1),
		    ImVec2(1, 0),
		    ImVec4(0, 0, 0, 0),
		    tint);
	};

	const bool editing_view = (m_mode == EditorViewMode::Editor);

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

		if (draw_toggle_button("##SnapGrid", m_snap_grid_icon, m_snap_enabled)) {
			m_snap_enabled = !m_snap_enabled;
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
			ImGui::SetTooltip("Toggle grid snap");
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(72.0f);
		if (m_gizmo_operation == ImGuizmo::ROTATE) {
			ImGui::DragFloat("##RotateSnap", &m_rotate_snap, 1.0f, 1.0f, 180.0f, "%.0f deg");
		} else if (m_gizmo_operation == ImGuizmo::SCALE) {
			ImGui::DragFloat("##ScaleSnap", &m_scale_snap, 0.01f, 0.01f, 10.0f, "%.2f");
		} else {
			ImGui::DragFloat("##TranslateSnap", &m_translate_snap, 0.1f, 0.1f, 100.0f, "%.2f");
		}

	}

	ImGui::EndGroup();
	ImGui::PopStyleVar(2);

	if (editing_view && active_scene && m_context.get_selected_entity().has_value()) {
		auto entity = *m_context.get_selected_entity();
		auto transform = active_scene->get_native_component<TransformComponent>(entity);
		if (transform) {
			glm::mat4 model = transform->get_model_matrix();
			auto render_view = m_context.ViewportController.get_render_view(m_mode, active_scene);
			glm::mat4 view = render_view.View;
			glm::mat4 proj = render_view.Projection;

			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(
			    viewport_min.x,
			    viewport_min.y,
			    viewport_size.x,
			    viewport_size.y);

			float snap_values[3] = { 0.0f, 0.0f, 0.0f };
			float* snap = nullptr;
			if (m_snap_enabled) {
				if (m_gizmo_operation == ImGuizmo::ROTATE) {
					snap_values[0] = m_rotate_snap;
				} else if (m_gizmo_operation == ImGuizmo::SCALE) {
					snap_values[0] = m_scale_snap;
					snap_values[1] = m_scale_snap;
					snap_values[2] = m_scale_snap;
				} else {
					snap_values[0] = m_translate_snap;
					snap_values[1] = m_translate_snap;
					snap_values[2] = m_translate_snap;
				}
				snap = snap_values;
			}

			ImGuizmo::Manipulate(
			    glm::value_ptr(view),
			    glm::value_ptr(proj),
			    m_gizmo_operation,
			    m_gizmo_mode,
			    glm::value_ptr(model),
			    nullptr,
			    snap);

			if (ImGuizmo::IsUsing())
				transform->set_from_matrix(model);
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	ImVec2 mouse = ImGui::GetMousePos();

	bool hovered = mouse.x >= viewport_min.x && mouse.y >= viewport_min.y && mouse.x <= viewport_max.x && mouse.y <= viewport_max.y;

	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		m_dragging = true;

	if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		m_dragging = false;

	if (ImGuizmo::IsUsing())
		m_dragging = false;

	if (editing_view && m_dragging && !resized && !ImGuizmo::IsUsing()) {
		ImVec2 delta = io.MouseDelta;

		delta.x = std::clamp(delta.x, -max_delta, max_delta);
		delta.y = std::clamp(delta.y, -max_delta, max_delta);

		m_smooth_delta.x = m_smooth_delta.x * (1.0f - smooth_alpha) + delta.x * smooth_alpha;
		m_smooth_delta.y = m_smooth_delta.y * (1.0f - smooth_alpha) + delta.y * smooth_alpha;

		glm::vec2 smooth_delta_glm {
			(m_smooth_delta * sensitivity).x,
			(m_smooth_delta * sensitivity).y
		};

		m_camera.on_mouse_delta(smooth_delta_glm);
	}
}
}
