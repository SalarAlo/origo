#include "panels/HierarchyPanel.h"

#include "imgui.h"
#include "origo/scene/Transform.h"

namespace OrigoEditor {
void HierarchyPanel::OnImGuiRender() {
	ImGui::Text("Scene Entities:");

	for (const auto& e : m_Scene.GetAllComponentsOfType<Origo::Transform>()) {
		if (ImGui::Selectable(e->AttachedTo->GetName().c_str(), m_Selected == e->AttachedTo.get()))
			m_Selected = e->AttachedTo.get();
	}
}
}
