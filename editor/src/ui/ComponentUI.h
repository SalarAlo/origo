#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "glm/fwd.hpp"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

namespace ComponentUI {

void DrawVec3Control(std::string_view label, glm::vec3& values, float speed = 0.1f);
void DrawVec2Control(std::string_view label, glm::vec2& values, float speed = 0.1f);
void DrawMinMaxRangeControl(std::string_view label, glm::vec2& values, float speed = 0.1f);

void DrawFloatControl(std::string_view label, float& value, float speed = 0.1f);
void DrawFloatControl(std::string_view label, float& value, float speed, const char* format);

void DrawIntControl(std::string_view label, int& value, float speed = 0.1f);

void DrawStringControl(std::string_view label, std::string& value);

void DrawBoolControl(std::string_view label, bool& value);

void DrawAssetControl(
    std::string_view label,
    Origo::OptionalAssetHandle& handle,
    std::optional<Origo::AssetType> assetValidationType = std::nullopt);

void DrawColorControl(std::string_view label, glm::vec3& value);

bool StartRegion(std::string_view label, bool defaultOpen = true);
void EndRegion(bool open);

template <typename T>
void DrawEnumControl(std::string_view label, T& value) {
	static_assert(std::is_enum_v<T>, "DrawEnumControl requires an enum type");

	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float fieldWidth = 140.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float nextX = ImGui::GetCursorPosX() + avail - fieldWidth;
	float minX = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;

	if (nextX < minX)
		nextX = minX;

	ImGui::SameLine(nextX);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(fieldWidth);

	const char* preview = magic_enum::enum_name(value).data();

	if (ImGui::BeginCombo("##enum", preview)) {
		for (auto v : magic_enum::enum_values<T>()) {
			bool selected = (v == value);

			if (ImGui::Selectable(magic_enum::enum_name(v).data(), selected))
				value = v;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	ImGui::PopStyleColor(4);
	ImGui::PopStyleVar();
	ImGui::PopID();
}

}
