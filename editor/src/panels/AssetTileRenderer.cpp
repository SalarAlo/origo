#include <cmath>

#include "panels/AssetTileRenderer.h"

#include "origo/assets/Asset.h"

#include "panels/EditorAssetTree.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

namespace {
	constexpr float k_tile_size = 96.0f;
	constexpr float k_tile_text_height = 22.0f;
	constexpr float k_tile_corner_rounding = 8.0f;
	constexpr float k_tile_thumbnail_corner_rounding = 6.0f;
}

void AssetTileRenderer::draw_folder_tile(ImDrawList* draw_list, const AssetTileLayout& layout, const FolderEntry& folder, bool hovered) const {
	draw_tile_background(draw_list, layout, get_folder_tile_background_color(hovered));
	draw_folder_thumbnail(draw_list, layout);

	const std::string display_name = clip_label(folder.Name, AssetLayout::get_tile_label_max_width());
	draw_tile_label(draw_list, layout, display_name);
	show_clipped_text_tooltip(hovered, display_name, folder.Name);
}

void AssetTileRenderer::draw_asset_tile(ImDrawList* draw_list, const AssetTileLayout& layout, const AssetEntry& asset, bool hovered, bool selected) const {
	draw_tile_background(draw_list, layout, get_asset_tile_background_color(selected, hovered));
	draw_asset_thumbnail(draw_list, layout, asset);

	const std::string display_name = clip_label(asset.Name, AssetLayout::get_tile_label_max_width());
	draw_tile_label(draw_list, layout, display_name);
	show_clipped_text_tooltip(hovered, display_name, asset.Name);
}

std::string AssetTileRenderer::clip_label(const std::string& text, float max_width) const {
	if (ImGui::CalcTextSize(text.c_str()).x <= max_width)
		return text;

	std::string out = text;
	while (!out.empty()) {
		out.pop_back();
		const std::string candidate = out + "...";
		if (ImGui::CalcTextSize(candidate.c_str()).x <= max_width)
			return candidate;
	}

	return "...";
}

void AssetTileRenderer::show_clipped_text_tooltip(bool hovered, const std::string& displayed, const std::string& full_text) const {
	if (!hovered || displayed == full_text)
		return;

	ImGui::BeginTooltip();
	ImGui::TextUnformatted(full_text.c_str());
	ImGui::EndTooltip();
}

ImVec2 AssetTileRenderer::get_centered_tile_label_position(const ImVec2& cursor, const std::string& label) {
	const ImVec2 text_size = ImGui::CalcTextSize(label.c_str());
	const float font_size = ImGui::GetFontSize();
	return {
		cursor.x + (k_tile_size - text_size.x) * 0.5f,
		cursor.y + k_tile_size + (k_tile_text_height - font_size) * 0.5f
	};
}

void AssetTileRenderer::draw_tile_background(ImDrawList* draw_list, const AssetTileLayout& layout, ImU32 color) {
	draw_list->AddRectFilled(layout.Cursor, layout.Cursor + layout.TileSize, color, k_tile_corner_rounding);
}

void AssetTileRenderer::draw_placeholder_thumbnail(ImDrawList* draw_list, const AssetTileLayout& layout) {
	draw_list->AddRectFilled(layout.ThumbnailMin, layout.ThumbnailMax, IM_COL32(90, 90, 90, 255), k_tile_thumbnail_corner_rounding);
}

void AssetTileRenderer::draw_tile_label(ImDrawList* draw_list, const AssetTileLayout& layout, const std::string& text) {
	const ImVec2 text_pos = get_centered_tile_label_position(layout.Cursor, text);
	draw_list->AddText(text_pos, IM_COL32(230, 230, 230, 255), text.c_str());
}

void AssetTileRenderer::add_image_rotated(ImDrawList* draw_list, ImTextureID texture, ImVec2 center, ImVec2 size, float angle_rad, ImU32 tint) {
	const ImVec2 half = size * 0.5f;
	const float c = cosf(angle_rad);
	const float s = sinf(angle_rad);

	auto rotate = [&](ImVec2 p) {
		p -= half;
		return ImVec2(
		    center.x + p.x * c - p.y * s,
		    center.y + p.x * s + p.y * c);
	};

	const ImVec2 p0 = rotate({ 0, 0 });
	const ImVec2 p1 = rotate({ size.x, 0 });
	const ImVec2 p2 = rotate({ size.x, size.y });
	const ImVec2 p3 = rotate({ 0, size.y });

	draw_list->AddImageQuad(
	    texture,
	    p0, p1, p2, p3,
	    ImVec2(0, 0),
	    ImVec2(1, 0),
	    ImVec2(1, 1),
	    ImVec2(0, 1),
	    tint);
}

ImU32 AssetTileRenderer::get_folder_tile_background_color(bool hovered) {
	return hovered ? IM_COL32(80, 80, 80, 140) : IM_COL32(50, 50, 50, 90);
}

ImU32 AssetTileRenderer::get_asset_tile_background_color(bool is_selected, bool hovered) {
	return is_selected ? IM_COL32(60, 120, 200, 180)
	    : hovered      ? IM_COL32(80, 80, 80, 140)
	                   : IM_COL32(50, 50, 50, 90);
}

void AssetTileRenderer::draw_folder_thumbnail(ImDrawList* draw_list, const AssetTileLayout& layout) const {
	ImTextureID icon = EditorIcons::get_instance().get(IconType::Folder);
	if (icon) {
		draw_list->AddImage(icon, layout.ThumbnailMin, layout.ThumbnailMax);
		return;
	}

	draw_placeholder_thumbnail(draw_list, layout);
}

void AssetTileRenderer::draw_asset_thumbnail(ImDrawList* draw_list, const AssetTileLayout& layout, const AssetEntry& asset) const {
	if (!asset.Icon) {
		draw_placeholder_thumbnail(draw_list, layout);
		return;
	}

	const ImVec2 size = layout.ThumbnailMax - layout.ThumbnailMin;
	const ImVec2 center = layout.ThumbnailMin + size * 0.5f;

	if (asset.Type == Origo::AssetType::Texture2D) {
		add_image_rotated(draw_list, asset.Icon, center, size, IM_PI);
		return;
	}

	add_image_rotated(draw_list, asset.Icon, center, size, 0.0f);
}

}
