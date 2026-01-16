#include "imgui.h"
#include "panels/HierarchyPanel.h"

#include "origo/components/Name.h"
#include "state/EditorContext.h"
#include "systems/EditorIcons.h"
#include <optional>

namespace OrigoEditor {

HierarchyPanel::HierarchyPanel(EditorContext& ctx)
    : m_Context(ctx) { }

void HierarchyPanel::OnImGuiRender() {
	ImGui::TextUnformatted("Hierarchy");
	ImGui::Separator();

	auto selectedEntityID { m_Context.GetSelectedEntity() };
	ImTextureID entityIcon { EditorIcons::Get(IconType::Entity) };

	ImGui::BeginChild("HierarchyList", ImVec2(0, 0), false);

	if (ImGui::BeginPopupContextWindow("HierarchyCreatePopup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
		ImGui::MenuItem("Empty");
		if (ImGui::IsItemClicked()) {
			m_Context.ActiveScene->CreateEntity("Entity");
		}

		ImGui::Separator();

		ImGui::MenuItem("Cube");
		if (ImGui::IsItemClicked()) {
			m_Context.ActiveScene->CreateEntity("Entity");
		}

		ImGui::MenuItem("Sphere");
		if (ImGui::IsItemClicked()) { }

		ImGui::MenuItem("Model");
		if (ImGui::IsItemClicked()) { }

		ImGui::EndPopup();
	}

	for (const auto& entityID : m_Context.ActiveScene->GetEntities()) {
		ImGui::PushID(entityID.GetId());

		bool selected = selectedEntityID && selectedEntityID == entityID;

		ImGui::Selectable("##EntitySelectable", selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !selected)
			m_Context.SetSelectedEntity(entityID);

		if (ImGui::BeginPopupContextItem("EntityContextPopup")) {
			ImGui::MenuItem("Remove");
			if (ImGui::IsItemClicked()) {
				m_Context.ActiveScene->ScheduleRemoveEntity(entityID);
				m_Context.UnselectEntity();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (entityIcon)
			ImGui::Image(entityIcon, ImVec2(16, 16));

		ImGui::SameLine(0.0f, 6.0f);

		auto* nameComp = m_Context.ActiveScene->GetNativeComponent<Origo::NameComponent>(entityID);
		ImGui::TextUnformatted(nameComp->GetName().c_str());

		ImGui::PopID();
	}

	ImGui::EndChild();
}

}
