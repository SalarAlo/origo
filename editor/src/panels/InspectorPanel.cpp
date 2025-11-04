#include "panels/InspectorPanel.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "origo/scene/Transform.h"

static void DrawVec3Control(const std::string& label, glm::vec3& values,
    float resetValue = 0.0f, float columnWidth = 100.0f, float speed = 0.1f) {
	ImGui::PushID(label.c_str());

	ImGui::TableNextColumn();
	ImGui::TextUnformatted(label.c_str());
	ImGui::TableNextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 0.0f });

	const float lineHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2.0f;
	const ImVec2 buttonSize(lineHeight + 4.0f, lineHeight);

	auto DrawAxisControl = [&](const char* id, float& value, const ImVec4& color) {
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.x + 0.1f, color.y + 0.1f, color.z + 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(color.x - 0.1f, color.y - 0.1f, color.z - 0.1f, 1.0f));

		if (ImGui::Button(id, buttonSize))
			value = resetValue;

		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat(std::string("##").append(id).c_str(), &value, speed, 0.0f, 0.0f, "%.3f");
		ImGui::PopItemWidth();
	};

	DrawAxisControl("X", values.x, ImVec4(0.85f, 0.25f, 0.15f, 1.0f));
	ImGui::SameLine();
	DrawAxisControl("Y", values.y, ImVec4(0.15f, 0.85f, 0.25f, 1.0f));
	ImGui::SameLine();
	DrawAxisControl("Z", values.z, ImVec4(0.15f, 0.25f, 0.85f, 1.0f));

	ImGui::PopStyleVar();
	ImGui::PopID();
}

namespace OrigoEditor {
void InspectorPanel::OnImGuiRender() {
	ImGui::Text("Scene Entities:");

	if (!m_SelectedEntity) {
		ImGui::TextDisabled("No entity selected.");
		return;
	}

	ImGui::SeparatorText(m_SelectedEntity->GetName().c_str());

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
		auto transform = m_Scene.GetComponent<Origo::Transform>(m_SelectedEntity->GetId());

		glm::vec3 pos = transform->GetPosition();
		glm::vec3 rot = transform->GetRotation();
		glm::vec3 scl = transform->GetScale();

		if (ImGui::BeginTable("TransformTable", 2, ImGuiTableFlags_SizingFixedFit)) {
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			ImGui::TableSetupColumn("Values", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextRow();
			DrawVec3Control("Position", pos, 0.0f, 100.0f);

			ImGui::TableNextRow();
			DrawVec3Control("Rotation", rot, 0.0f, 100.0f);

			ImGui::TableNextRow();
			DrawVec3Control("Scale", scl, 1.0f, 100.0f);

			ImGui::EndTable();
		}

		transform->SetPosition(pos);
		transform->SetRotation(rot);
		transform->SetScale(scl);
	}
}

}
