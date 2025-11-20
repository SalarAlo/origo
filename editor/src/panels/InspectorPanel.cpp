#include "panels/InspectorPanel.h"

#include "imgui.h"
#include "ui/InspectorDrawableRegistry.h"

namespace OrigoEditor {

void InspectorPanel::OnImGuiRender() {
	auto& selectedEntity { m_Contex.SelectedEntity };
	auto& scene { m_Contex.Scene };
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 4.0f, 3.0f });
	ImGui::SetWindowFontScale(0.9f);

	if (!selectedEntity) {
		ImGui::TextDisabled("No entity selected.");
		ImGui::PopStyleVar(2);
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	ImGui::SeparatorText(selectedEntity->GetName().c_str());

	for (auto* comp : scene.GetComponents(selectedEntity->GetId())) {
		InspectorDrawRegistry::Draw(comp, typeid(*comp));
	}

	ImGui::PopStyleVar(2);
	ImGui::SetWindowFontScale(1.0f);
}

}
