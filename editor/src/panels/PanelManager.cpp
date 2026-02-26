#include "panels/PanelManager.h"

#include "imgui.h"

#include "components/EditorOutline.h"

#include "origo/components/Transform.h"

#include "state/EditorContext.h"

#include "ui/UI.h"

namespace OrigoEditor {

PanelManager::PanelManager(EditorContext& context)
    : m_context(context) {
}

void PanelManager::render_menu_items() {
	if (ImGui::BeginMenu("View")) {
		if (ImGui::BeginMenu("Panels")) {
			for (const auto& panel : m_panels) {
				ImGui::MenuItem(panel->get_name(), nullptr, &panel->is_open_ref());
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Palette")) {
			for (const auto& palette : OrigoEditor::get_all_editor_palettes()) {
				bool selected = (palette.Name == m_context.ColorPalette.Name);
				if (ImGui::MenuItem(palette.Name.c_str(), nullptr, selected)) {
					m_context.ColorPalette = palette;
					UI::apply_editor_style(m_context.ColorPalette);
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Entities")) {
		static int i {};
		if (ImGui::MenuItem("Create Entity")) {
			auto e = m_context.ActiveScene->create_entity("entity_" + std::to_string(i++));
			m_context.ActiveScene->add_native_component<OrigoEditor::EditorOutlineComponent>(e);
		}

		if (ImGui::MenuItem("Delete Entity")) {
		}

		ImGui::EndMenu();
	}
}

void PanelManager::render_panels() {
	for (const auto& panel : m_panels) {
		if (panel->is_open()) {

			ImGuiWindowFlags flags = 0;

			if (!panel->is_collapsable())
				flags |= ImGuiWindowFlags_NoCollapse;

			ImGui::Begin(panel->get_name(), nullptr, flags);
			panel->on_im_gui_render();
			ImGui::End();
		}
	}
}

}
