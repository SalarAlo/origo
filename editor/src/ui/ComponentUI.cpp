#include "ui/ComponentUI.h"

#include "imgui.h"
#include "glm/glm.hpp"

namespace ComponentUI {
void DrawVec3Control(std::string_view label, glm::vec3& values, float speed) {
	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 65.0f;
	const float spacingBetweenFields = 4.0f;

	ImGui::BeginGroup();

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float totalFieldWidth = fieldWidth * 3.0f + spacingBetweenFields * 2.0f;

	float rightMargin = 5.0f;
	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - totalFieldWidth - rightMargin;
	if (nextX < ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f)
		nextX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;
	ImGui::SameLine(nextX);

	auto DrawField = [&](const char* id, float& v) {
		ImVec4 textColor(0.85f, 0.85f, 0.85f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, textColor);

		ImGui::SetNextItemWidth(fieldWidth);
		ImGui::DragFloat(std::string("##").append(id).c_str(), &v, speed, 0.0f, 0.0f, "%.2f");

		ImGui::PopStyleColor(4);
	};

	DrawField("X", values.x);
	ImGui::SameLine(0.0f, spacingBetweenFields);
	DrawField("Y", values.y);
	ImGui::SameLine(0.0f, spacingBetweenFields);
	DrawField("Z", values.z);

	ImGui::EndGroup();

	ImGui::PopStyleVar();
	ImGui::PopID();
}
}
