#include "panels/PanelManager.h"
#include "state/EditorContext.h"
#include "imgui.h"
#include "origo/scene/Transform.h"
#include "ui/UI.h"

namespace OrigoEditor {

PanelManager::PanelManager(EditorContext& context)
    : m_Context(context) {
}

void PanelManager::RenderMenuItems() {
	if (ImGui::BeginMenu("View")) {
		if (ImGui::BeginMenu("Panels")) {
			for (const auto& panel : m_Panels) {
				ImGui::MenuItem(panel->GetName(), nullptr, &panel->IsOpenRef());
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Palette")) {
			for (const auto& palette : OrigoEditor::GetAllEditorPalettes()) {
				bool selected = (palette.Name == m_Context.ColorPalette.Name);
				if (ImGui::MenuItem(palette.Name.c_str(), nullptr, selected)) {
					m_Context.ColorPalette = palette;
					UI::ApplyEditorStyle(m_Context.ColorPalette);
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Entities")) {
		static int i {};
		if (ImGui::MenuItem("Create Entity")) {
			auto e = m_Context.EditorScene.CreateEntity("entity_" + std::to_string(i++));
			m_Context.EditorScene.AddComponent<Origo::Transform>(e);
		}

		if (ImGui::MenuItem("Delete Entity")) {
		}

		ImGui::EndMenu();
	}
}

void PanelManager::RenderPanels() {
	for (const auto& panel : m_Panels) {
		if (panel->IsOpen()) {
			ImGui::Begin(panel->GetName());
			panel->OnImGuiRender();
			ImGui::End();
		}
	}
}

}
