#include "panels/PanelManager.h"
#include "imgui.h"

namespace OrigoEditor {

void PanelManager::RenderMenuItems() {
	if (ImGui::BeginMenu("View")) {
		for (const auto& panel : m_Panels) {
			ImGui::MenuItem(panel->GetName(), nullptr, &panel->IsOpenRef());
		}
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
