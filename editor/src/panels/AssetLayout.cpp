#include "panels/AssetLayout.h"

#include <algorithm>

namespace OrigoEditor {

namespace {
	constexpr float k_tile_size = 96.0f;
	constexpr float k_tile_padding = 10.0f;
	constexpr float k_tile_text_height = 22.0f;
	constexpr float k_tile_thumbnail_inset = 10.0f;
	constexpr float k_tile_text_horizontal_padding = 6.0f;
	constexpr float k_search_width = 220.0f;
	constexpr float k_breadcrumb_item_spacing_x = 6.0f;
	constexpr float k_breadcrumb_max_segment_width = 220.0f;

	const char* get_folder_name_or_root_label(const FolderEntry* folder) {
		return folder->Name.empty() ? "Assets" : folder->Name.c_str();
	}
}

AssetTileLayout AssetLayout::make_tile_layout() {
	const ImVec2 cursor = ImGui::GetCursorScreenPos();
	const ImVec2 tile_size = { k_tile_size, k_tile_size + k_tile_text_height };
	const ImVec2 thumbnail_min = cursor + ImVec2(k_tile_thumbnail_inset, k_tile_thumbnail_inset);
	const ImVec2 thumbnail_max = thumbnail_min + ImVec2(k_tile_size - k_tile_thumbnail_inset * 2.0f, k_tile_size - k_tile_thumbnail_inset * 2.0f);

	return {
		cursor,
		tile_size,
		thumbnail_min,
		thumbnail_max
	};
}

int AssetLayout::calculate_tile_columns(float panel_width) {
	const float cell_size = k_tile_size + k_tile_padding;
	return std::max(1, (int)(panel_width / cell_size));
}

float AssetLayout::calculate_breadcrumb_total_width(const std::vector<FolderEntry*>& path) {
	const ImGuiStyle& style = ImGui::GetStyle();

	float total_width = 0.0f;
	for (size_t i = 0; i < path.size(); ++i) {
		const char* label = get_folder_name_or_root_label(path[i]);
		total_width += ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
		if (i + 1 < path.size())
			total_width += ImGui::CalcTextSize(">").x + style.ItemSpacing.x * 2.0f;
	}

	return total_width;
}

float AssetLayout::get_search_width() {
	return k_search_width;
}

float AssetLayout::get_breadcrumb_item_spacing_x() {
	return k_breadcrumb_item_spacing_x;
}

float AssetLayout::get_breadcrumb_max_segment_width() {
	return k_breadcrumb_max_segment_width;
}

float AssetLayout::get_tile_label_max_width() {
	return k_tile_size - k_tile_text_horizontal_padding * 2.0f;
}

}
