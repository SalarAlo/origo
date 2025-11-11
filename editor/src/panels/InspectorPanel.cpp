#include "panels/InspectorPanel.h"

#include "imgui.h"
#include "ui/ComponentDrawRegistry.h"

namespace OrigoEditor {

void InspectorPanel::OnImGuiRender() {
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 4.0f, 3.0f });
	ImGui::SetWindowFontScale(0.9f);

	ImGui::TextUnformatted("Scene Entities:");

	if (!m_SelectedEntity) {
		ImGui::TextDisabled("No entity selected.");
		ImGui::PopStyleVar(2);
		ImGui::SetWindowFontScale(1.0f);
		return;
	}

	ImGui::SeparatorText(m_SelectedEntity->GetName().c_str());

	for (auto* comp : m_Scene.GetComponents(m_SelectedEntity->GetId())) {
		ComponentDrawRegistry::Draw(comp, typeid(*comp));
	}

	ImGui::PopStyleVar(2);
	ImGui::SetWindowFontScale(1.0f);
}

} // namespace OrigoEditor
