#pragma once

#include <imgui.h>

#include <string>

#include "panels/AssetLayout.h"

namespace OrigoEditor {

struct AssetEntry;
struct FolderEntry;

class AssetTileRenderer {
public:
	void draw_folder_tile(ImDrawList* draw_list, const AssetTileLayout& layout, const FolderEntry& folder, bool hovered) const;
	void draw_asset_tile(ImDrawList* draw_list, const AssetTileLayout& layout, const AssetEntry& asset, bool hovered, bool selected) const;
	std::string clip_label(const std::string& text, float max_width) const;
	void show_clipped_text_tooltip(bool hovered, const std::string& displayed, const std::string& full_text) const;

private:
	static ImVec2 get_centered_tile_label_position(const ImVec2& cursor, const std::string& label);
	static void draw_tile_background(ImDrawList* draw_list, const AssetTileLayout& layout, ImU32 color);
	static void draw_placeholder_thumbnail(ImDrawList* draw_list, const AssetTileLayout& layout);
	static void draw_tile_label(ImDrawList* draw_list, const AssetTileLayout& layout, const std::string& text);
	static void add_image_rotated(ImDrawList* draw_list, ImTextureID texture, ImVec2 center, ImVec2 size, float angle_rad, ImU32 tint = IM_COL32_WHITE);
	static ImU32 get_folder_tile_background_color(bool hovered);
	static ImU32 get_asset_tile_background_color(bool is_selected, bool hovered);
	void draw_folder_thumbnail(ImDrawList* draw_list, const AssetTileLayout& layout) const;
	void draw_asset_thumbnail(ImDrawList* draw_list, const AssetTileLayout& layout, const AssetEntry& asset) const;
};

}
