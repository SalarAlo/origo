#include "panels/InspectorPanel.h"

#include "ui/InspectorDrawableRegistry.h"

namespace OrigoEditor {

void InspectorPanel::OnImGuiRender() {
	auto& selectedEntity = m_Contex.SelectedEntity;
	auto scene = m_Contex.ActiveScene;

	ImGui::SetWindowFontScale(1.1f);

	if (!selectedEntity.has_value()) {
		ImGui::TextDisabled("No entity selected.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	Origo::RID entity = selectedEntity->GetId();

	ImGui::SeparatorText(selectedEntity->GetName().c_str());
	ImGui::SetWindowFontScale(0.9f);

	for (const auto& [type, entry] : InspectorDrawRegistry::GetEntries()) {
		if (!scene->HasComponentByType(entity, type))
			continue;

		void* componentPtr = scene->GetComponentByType(entity, type);
		InspectorDrawRegistry::Draw(componentPtr, type);
	}

	ImGui::SetWindowFontScale(1.0f);
}

}
