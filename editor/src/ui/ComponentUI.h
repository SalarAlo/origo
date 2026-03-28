#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include "imgui.h"

#include "glm/fwd.hpp"

#include "magic_enum/magic_enum.hpp"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

namespace ComponentUI {

namespace detail {

	inline constexpr ImVec2 k_control_item_spacing { 4.0f, 4.0f };
	inline constexpr float k_label_spacing = 8.0f;

	inline float get_control_row_offset(std::string_view label, float width) {
		const float current_x = ImGui::GetCursorPosX();
		const float available_width = ImGui::GetContentRegionAvail().x;
		const float label_width = ImGui::CalcTextSize(label.data()).x;
		return std::max(current_x + available_width - width, current_x + label_width + k_label_spacing);
	}

	inline void begin_control_row(std::string_view label, float width) {
		ImGui::PushID(label.data());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, k_control_item_spacing);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label.data());
		ImGui::SameLine(get_control_row_offset(label, width));
	}

	inline void end_control_row() {
		ImGui::PopStyleVar();
		ImGui::PopID();
	}

	inline void push_frame_style() {
		const ImVec4* colors = ImGui::GetStyle().Colors;
		ImGui::PushStyleColor(ImGuiCol_FrameBg, colors[ImGuiCol_FrameBg]);
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, colors[ImGuiCol_FrameBgHovered]);
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, colors[ImGuiCol_FrameBgActive]);
		ImGui::PushStyleColor(ImGuiCol_Text, colors[ImGuiCol_Text]);
	}

	inline void pop_frame_style() {
		ImGui::PopStyleColor(4);
	}

}

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

	const float field_width = 140.0f;
	detail::begin_control_row(label, field_width);
	detail::push_frame_style();
	ImGui::SetNextItemWidth(field_width);

	if (const char* preview = magic_enum::enum_name(value).data(); ImGui::BeginCombo("##enum", preview)) {
		for (auto v : magic_enum::enum_values<T>()) {
			const bool selected = (v == value);

			if (ImGui::Selectable(magic_enum::enum_name(v).data(), selected))
				value = v;

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	detail::pop_frame_style();
	detail::end_control_row();
}

}
