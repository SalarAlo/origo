#include "panels/PanelManager.h"
#include "EditorContext.h"
#include "origo/scene/Transform.h"

namespace OrigoEditor {

PanelManager::PanelManager(EditorContext& context)
    : m_Context(context) {
}

void PanelManager::RenderMenuItems() {
	if (ImGui::BeginMenu("View")) {
		for (const auto& panel : m_Panels) {
			ImGui::MenuItem(panel->GetName(), nullptr, &panel->IsOpenRef());
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Entities")) {
		static int i {};
		if (ImGui::MenuItem("Create Entity")) {
			auto e = m_Context.Scene.CreateEntity("entity_" + std::to_string(i++));
			m_Context.Scene.AddComponent<Origo::Transform>(e);
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
