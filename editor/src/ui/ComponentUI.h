#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "glm/fwd.hpp"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

namespace ComponentUI {

void draw_vec3_control(std::string_view label, glm::vec3& values, float speed = 0.1f);
void draw_vec2_control(std::string_view label, glm::vec2& values, float speed = 0.1f);
void draw_min_max_range_control(std::string_view label, glm::vec2& values, float speed = 0.1f);

void draw_float_control(std::string_view label, float& value, float speed = 0.1f);
void draw_float_control(std::string_view label, float& value, float speed, const char* format);
void draw_float_slider_control(std::string_view label, float& value, float min, float max, const char* format = "%.3f");

void draw_int_control(std::string_view label, int& value, float speed = 0.1f);

void draw_string_control(std::string_view label, std::string& value);

void draw_bool_control(std::string_view label, bool& value);

void draw_asset_control(
    std::string_view label,
    Origo::OptionalAssetHandle& handle,
    std::optional<Origo::AssetType> assetValidationType = std::nullopt);

void draw_color_control(std::string_view label, glm::vec3& value);

bool start_region(std::string_view label, bool defaultOpen = true);
void end_region(bool open);

template <typename T>
void draw_enum_control(std::string_view label, T& value) {
	static_assert(std::is_enum_v<T>, "DrawEnumControl requires an enum type");

	ImGui::PushID(label.data());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2 { 4.0f, 4.0f });

	const float field_width = 140.0f;

	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label.data());

	float avail = ImGui::GetContentRegionAvail().x;
	float next_x = ImGui::GetCursorPosX() + avail - field_width;
	float min_x = ImGui::GetCursorPosX() + ImGui::CalcTextSize(label.data()).x + 8.0f;

	if (next_x < min_x)
		next_x = min_x;

	ImGui::SameLine(next_x);

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.13f, 0.13f, 0.13f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.17f, 0.17f, 0.17f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));

	ImGui::SetNextItemWidth(field_width);

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
