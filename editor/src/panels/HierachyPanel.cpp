#include "components/EditorMeshRenderer.h"
#include "panels/HierarchyPanel.h"

#include "imgui.h"
#include "origo/scene/Transform.h"

namespace OrigoEditor {
void HierarchyPanel::OnImGuiRender() {
	ImGui::Text("Scene Entities:");

	auto& selectedEntity { m_Context.SelectedEntity };
	for (const auto& transform : m_Context.Scene.GetAllComponentsOfType<Origo::Transform>()) {
		auto& entity { *transform->AttachedTo };
		bool isEntitySelected { selectedEntity.has_value() && selectedEntity.value().GetId() == entity.GetId() };

		if (ImGui::Selectable(entity.GetName().c_str(), isEntitySelected)) {
			ChangeActiveSelectedEntity(entity);
		}
	}
}
void HierarchyPanel::ChangeActiveSelectedEntity(Origo::Entity& e) {
	if (m_Context.SelectedEntity.has_value()) {
		auto emr = m_Context.Scene.GetComponent<EditorMeshRenderer>(m_Context.SelectedEntity.value().GetId());
		emr->IsSelected = false;
	}

	m_Context.SelectedEntity = e;
	auto newEmr = m_Context.Scene.GetComponent<EditorMeshRenderer>(m_Context.SelectedEntity->GetId());
	newEmr->IsSelected = true;
}

}
