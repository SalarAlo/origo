#include "panels/InspectorPanel.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "origo/scene/Transform.h"

static void DrawVec3Control(const std::string& label, glm::vec3& values,
    float resetValue = 0.0f, float columnWidth = 100.0f) {
	ImGui::PushID(label.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 0, 0 });

	float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	// X
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.8f, 0.1f, 0.15f, 1.0f });
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// Y
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.2f, 0.7f, 0.2f, 1.0f });
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// Z
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4 { 0.1f, 0.25f, 0.8f, 1.0f });
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f);
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();
	ImGui::Columns(1);
	ImGui::PopID();
}

namespace OrigoEditor {
void InspectorPanel::OnImGuiRender() {
	ImGui::Text("Scene Entities:");

	if (!m_SelectedEntity) {
		ImGui::TextDisabled("No entity selected.");
		return;
	}

	ImGui::Text("Entity: %s", m_SelectedEntity->GetName().c_str());
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		auto transform = m_Scene.GetComponent<Origo::Transform>(m_SelectedEntity->GetId());
		glm::vec3 pos = transform->GetPosition();
		glm::vec3 rot = transform->GetRotation();
		glm::vec3 scl = transform->GetScale();

		DrawVec3Control("Position", pos);
		DrawVec3Control("Rotation", rot);
		DrawVec3Control("Scale", scl, 1.0f);

		transform->SetPosition(pos);
		transform->SetRotation(rot);
		transform->SetScale(scl);
	}
}

}
