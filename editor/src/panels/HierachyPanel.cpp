#include "components/EditorMeshRenderer.h"
#include "origo/assets/AssetManagerFast.h"
#include "panels/HierarchyPanel.h"

#include "imgui.h"
#include "origo/scene/Transform.h"

namespace OrigoEditor {
void HierarchyPanel::OnImGuiRender() {
	ImGui::Text("Scene Entities:");

	for (const auto& e : m_Context.Scene.GetAllComponentsOfType<Origo::Transform>())
		if (ImGui::Selectable(e->AttachedTo->GetName().c_str(), m_Context.SelectedEntity == e->AttachedTo)) {
			if (m_Context.SelectedEntity) {
				auto emr = m_Context.Scene.GetComponent<EditorMeshRenderer>(m_Context.SelectedEntity->GetId());
				emr->IsSelected = false;
			}
			m_Context.SelectedEntity = e->AttachedTo;
			auto newEmr = m_Context.Scene.GetComponent<EditorMeshRenderer>(m_Context.SelectedEntity->GetId());
			newEmr->IsSelected = true;
		}
}
}
