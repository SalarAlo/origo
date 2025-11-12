#include "panels/HierarchyPanel.h"

#include "imgui.h"
#include "origo/scene/Transform.h"

namespace OrigoEditor {
void HierarchyPanel::OnImGuiRender() {
	ImGui::Text("Scene Entities:");

	for (const auto& e : m_Context.Scene.GetAllComponentsOfType<Origo::Transform>())
		if (ImGui::Selectable(e->AttachedTo->GetName().c_str(), m_Context.SelectedEntity == e->AttachedTo))
			m_Context.SelectedEntity = e->AttachedTo;
}
}
