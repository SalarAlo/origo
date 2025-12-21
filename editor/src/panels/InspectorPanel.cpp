#include "panels/InspectorPanel.h"

#include "ui/InspectorDrawableRegistry.h"

namespace OrigoEditor {

void InspectorPanel::OnImGuiRender() {
	auto& selectedEntity { m_Contex.SelectedEntity };
	auto& scene { m_Contex.Scene };
	ImGui::SetWindowFontScale(1.1f);

	if (!selectedEntity.has_value()) {
		ImGui::TextDisabled("No entity selected.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	ImGui::SeparatorText(selectedEntity.value().GetName().c_str());

	ImGui::SetWindowFontScale(0.9f);

	for (auto* comp : scene.GetComponents(selectedEntity.value().GetId())) {
		InspectorDrawRegistry::Draw(comp, typeid(*comp));
	}

	ImGui::SetWindowFontScale(1.0f);
}

}
