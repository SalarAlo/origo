#include "panels/InspectorPanel.h"

#include "origo/scene/ComponentManager.h"
#include "origo/scene/ComponentRegistry.h"
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

	Origo::RID entity = selectedEntity->GetID();

	ImGui::SeparatorText(selectedEntity->GetName().c_str());
	ImGui::SetWindowFontScale(0.9f);

	for (const auto& [type, entry] : InspectorDrawRegistry::GetEntries()) {
		if (!scene->HasComponentByType(entity, type))
			continue;

		void* componentPtr = scene->GetComponentByType(entity, type);
		InspectorDrawRegistry::Draw(componentPtr, type);
	}

	ImGui::SetWindowFontScale(1.0f);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	float buttonWidth = 180.0f;

	float contentWidth = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;

	ImGui::SetCursorPosX(
	    ImGui::GetWindowContentRegionMin().x + (contentWidth - buttonWidth) * 0.5f);

	static ImVec2 addButtonPos;
	static ImVec2 addButtonSize;

	if (ImGui::Button("+ Add Component", ImVec2(buttonWidth, 0))) {
		addButtonPos = ImGui::GetItemRectMin();
		addButtonSize = ImGui::GetItemRectSize();
		ImGui::OpenPopup("AddComponentPopup");
	}

	ImVec2 popupPos = {
		addButtonPos.x,
		addButtonPos.y + addButtonSize.y
	};

	ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(300.0f, 0.0f), ImGuiCond_Appearing);

	if (ImGui::BeginPopup("AddComponentPopup",
	        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {

		ImGui::TextDisabled("Available Components");
		ImGui::Separator();

		for (const auto& [type, entry] : InspectorDrawRegistry::GetEntries()) {

			if (scene->HasComponentByType(entity, type))
				continue;

			bool isRegistered = Origo::ComponentRegistry::Get(type) != nullptr;

			if (!isRegistered) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
				ImGui::BeginDisabled();

				ImGui::MenuItem(entry.Name);

				ImGui::EndDisabled();
				ImGui::PopStyleColor();
				continue;
			}

			if (ImGui::MenuItem(entry.Name)) {
				ImGui::CloseCurrentPopup();
				scene->AddComponent(entity, type);
				break;
			}
		}

		ImGui::EndPopup();
	}
}

}
