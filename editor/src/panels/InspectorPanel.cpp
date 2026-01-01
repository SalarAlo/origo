#include "panels/InspectorPanel.h"

#include "imgui.h"
#include "origo/scene/Name.h"
#include "origo/scene/NativeComponentRegistry.h"
#include "origo/scripting/ScriptComponentRegistry.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

void InspectorPanel::OnImGuiRender() {
	auto& selectedEntityOptional = m_Context.SelectedEntity;
	auto activeScene = m_Context.ActiveScene;

	if (!selectedEntityOptional.has_value()) {
		ImGui::TextDisabled("No entity selected.");
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	DrawEntityName();

	const Origo::RID selectedEntity = selectedEntityOptional.value();

	ImGui::SetWindowFontScale(0.9f);

	DrawNativeComponents(activeScene, selectedEntity);
	DrawScriptComponents(activeScene, selectedEntity);

	ImGui::SetWindowFontScale(1.0f);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	DrawAddComponent(activeScene, selectedEntity);
}

void InspectorPanel::DrawEntityName() {
	auto& selectedEntity = m_Context.SelectedEntity;
	auto scene = m_Context.ActiveScene;

	const Origo::RID entity = selectedEntity.value();
	auto* nameComp = scene->GetNativeComponent<Origo::Name>(entity);
	const std::string& name = nameComp->GetName();

	static Origo::RID editingEntity {};
	static bool isEditingName = false;
	static char nameBuffer[256] {};

	ImGui::SetWindowFontScale(1.1f);

	if (!isEditingName || editingEntity != entity) {
		isEditingName = false;
	}

	if (!isEditingName) {
		ImGui::SeparatorText(name.c_str());

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			editingEntity = entity;
			isEditingName = true;
			std::strncpy(nameBuffer, name.c_str(), sizeof(nameBuffer));
			nameBuffer[sizeof(nameBuffer) - 1] = '\0';
			ImGui::SetKeyboardFocusHere();
		}
	} else {
		ImGui::SetNextItemWidth(-1.0f);

		const bool enterPressed = ImGui::InputText("##EntityName",
		    nameBuffer,
		    sizeof(nameBuffer),
		    ImGuiInputTextFlags_EnterReturnsTrue);

		const bool deactivated = ImGui::IsItemDeactivatedAfterEdit();

		if (enterPressed || deactivated) {
			nameComp->SetName(nameBuffer);
			isEditingName = false;
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			std::strncpy(nameBuffer, name.c_str(), sizeof(nameBuffer));
			isEditingName = false;
		}
	}
}

void InspectorPanel::DrawNativeComponents(Origo::Scene* activeScene, Origo::RID selectedEntity) {
	for (const auto& [type, entry] : InspectorDrawRegistry::GetEntries()) {
		if (!activeScene->HasNativeComponentByType(selectedEntity, type))
			continue;

		void* componentPtr = activeScene->GetNativeComponentByType(selectedEntity, type);
		InspectorDrawRegistry::DrawNativeComponent(componentPtr, type);
	}
}

void InspectorPanel::DrawScriptComponents(Origo::Scene* activeScene, Origo::RID selectedEntity) {
	for (const auto& [id, descr] : Origo::ScriptComponentRegistry::GetAll()) {
		if (!activeScene->HasScriptComponent(selectedEntity, id))
			continue;

		auto* instance = activeScene->GetScriptComponent(selectedEntity, id);
		InspectorDrawRegistry::DrawScriptComponent(*instance);
	}
}

void InspectorPanel::DrawAddComponent(Origo::Scene* activeScene, Origo::RID selectedEntity) {
	const float buttonWidth = 180.0f;
	const float contentWidth = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;

	ImGui::SetCursorPosX(
	    ImGui::GetWindowContentRegionMin().x + (contentWidth - buttonWidth) * 0.5f);

	static ImVec2 addButtonPos {};
	static ImVec2 addButtonSize {};

	if (ImGui::Button("+ Add Component", ImVec2(buttonWidth, 0))) {
		addButtonPos = ImGui::GetItemRectMin();
		addButtonSize = ImGui::GetItemRectSize();
		ImGui::OpenPopup("AddComponentPopup");
	}

	ImGui::SetNextWindowPos(
	    ImVec2(addButtonPos.x, addButtonPos.y + addButtonSize.y),
	    ImGuiCond_Appearing);

	ImGui::SetNextWindowSize(ImVec2(300.0f, 0.0f),
	    ImGuiCond_Appearing);

	if (ImGui::BeginPopup("AddComponentPopup",
	        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {

		ImGui::TextDisabled("Available Components");
		ImGui::Separator();

		for (const auto& [type, entry] : InspectorDrawRegistry::GetEntries()) {
			if (activeScene->HasNativeComponentByType(selectedEntity, type))
				continue;

			const bool isRegistered = Origo::NativeComponentRegistry::Get(type) != nullptr;

			if (!isRegistered) {
				ImGui::PushStyleColor(
				    ImGuiCol_Text,
				    ImVec4(0.85f, 0.25f, 0.25f, 1.0f));
				ImGui::BeginDisabled();
				ImGui::MenuItem(entry.Name);
				ImGui::EndDisabled();
				ImGui::PopStyleColor();
				continue;
			}

			if (ImGui::MenuItem(entry.Name)) {
				activeScene->AddNativeComponent(selectedEntity, type);
				ImGui::CloseCurrentPopup();
				break;
			}
		}

		ImGui::Separator();
		ImGui::TextDisabled("Script Components");

		for (const auto& [id, desc] : Origo::ScriptComponentRegistry::GetAll()) {

			if (activeScene->HasScriptComponent(selectedEntity, id))
				continue;

			if (ImGui::MenuItem(desc.Name.c_str())) {
				activeScene->AddScriptComponent(selectedEntity, id);
				ImGui::CloseCurrentPopup();
				break;
			}
		}

		ImGui::EndPopup();
	}
}
}
