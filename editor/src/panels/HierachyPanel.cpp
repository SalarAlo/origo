#include "panels/HierarchyPanel.h"

#include "origo/scene/Name.h"
#include "state/EditorContext.h"
#include "systems/EditorIcons.h"

namespace OrigoEditor {

HierarchyPanel::HierarchyPanel(EditorContext& ctx)
    : m_Context(ctx) { }

void HierarchyPanel::OnImGuiRender() {
	ImGui::TextUnformatted("Scene Entities:");

	ImTextureID entityIcon = EditorIcons::Get(IconType::Entity);
	auto selectedEntityID = m_Context.GetSelectedEntity();

	for (const auto& entityID : m_Context.ActiveScene->GetEntities()) {
		bool selected = selectedEntityID && selectedEntityID == entityID;

		if (entityIcon)
			ImGui::Image(entityIcon, ImVec2(16, 16));
		else
			ImGui::Dummy(ImVec2(16, 16));

		ImGui::SameLine();

		auto* nameComp = m_Context.ActiveScene->GetNativeComponent<Origo::Name>(entityID);

		if (ImGui::Selectable(
		        nameComp->GetName().c_str(),
		        selected)
		    && selectedEntityID != entityID) {

			m_Context.SetSelectedEntity(entityID);
		}
	}
}

}
