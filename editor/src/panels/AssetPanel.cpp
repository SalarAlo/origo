#include <cctype>
#include <cmath>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <filesystem>
#include <string>

#include "panels/AssetPanel.h"

#include "imgui_internal.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/serialization/JsonSerializer.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

namespace {
	constexpr float k_tile_size = 96.0f;
	constexpr float k_tile_padding = 10.0f;
	constexpr float k_tile_text_height = 22.0f;
	constexpr float k_tile_corner_rounding = 8.0f;
	constexpr float k_tile_thumbnail_inset = 10.0f;
	constexpr float k_tile_thumbnail_corner_rounding = 6.0f;
	constexpr float k_tile_text_horizontal_padding = 6.0f;
	constexpr float k_search_width = 220.0f;
	constexpr float k_breadcrumb_item_spacing_x = 6.0f;
	constexpr float k_breadcrumb_max_segment_width = 220.0f;

	struct TileLayout {
		ImVec2 Cursor;
		ImVec2 TileSize;
		ImVec2 ThumbnailMin;
		ImVec2 ThumbnailMax;
	};

	static std::string clip_text_to_width(const std::string& text, float maxWidth) {
		if (ImGui::CalcTextSize(text.c_str()).x <= maxWidth)
			return text;

		std::string out = text;
		while (!out.empty()) {
			out.pop_back();
			std::string test = out + "...";
			if (ImGui::CalcTextSize(test.c_str()).x <= maxWidth)
				return test;
		}
		return "...";
	}

	static bool contains_case_insensitive(const std::string& haystack, const std::string& needle) {
		if (needle.empty())
			return true;

		auto lower = [](unsigned char c) { return (char)std::tolower(c); };

		std::string h;
		h.reserve(haystack.size());
		for (char c : haystack)
			h.push_back(lower((unsigned char)c));

		std::string n;
		n.reserve(needle.size());
		for (char c : needle)
			n.push_back(lower((unsigned char)c));

		return h.find(n) != std::string::npos;
	}

	static TileLayout make_tile_layout() {
		const ImVec2 cursor = ImGui::GetCursorScreenPos();
		const ImVec2 tile_size = { k_tile_size, k_tile_size + k_tile_text_height };

		const ImVec2 thumbnail_min = cursor + ImVec2(k_tile_thumbnail_inset, k_tile_thumbnail_inset);
		const ImVec2 thumbnail_max = thumbnail_min + ImVec2(k_tile_size - k_tile_thumbnail_inset * 2.0f, k_tile_size - k_tile_thumbnail_inset * 2.0f);

		return {
			.Cursor = cursor,
			.TileSize = tile_size,
			.ThumbnailMin = thumbnail_min,
			.ThumbnailMax = thumbnail_max
		};
	}

	static float get_tile_label_max_width() {
		return k_tile_size - k_tile_text_horizontal_padding * 2.0f;
	}

	static ImVec2 get_centered_tile_label_position(const ImVec2& cursor, const std::string& label) {
		const ImVec2 text_size = ImGui::CalcTextSize(label.c_str());
		const float font_size = ImGui::GetFontSize();
		return {
			cursor.x + (k_tile_size - text_size.x) * 0.5f,
			cursor.y + k_tile_size + (k_tile_text_height - font_size) * 0.5f
		};
	}

	static void draw_tile_background(ImDrawList* draw_list, const TileLayout& layout, ImU32 color) {
		draw_list->AddRectFilled(layout.Cursor, layout.Cursor + layout.TileSize, color, k_tile_corner_rounding);
	}

	static void draw_placeholder_thumbnail(ImDrawList* draw_list, const TileLayout& layout) {
		draw_list->AddRectFilled(layout.ThumbnailMin, layout.ThumbnailMax, IM_COL32(90, 90, 90, 255), k_tile_thumbnail_corner_rounding);
	}

	static void draw_tile_label(ImDrawList* draw_list, const TileLayout& layout, const std::string& text) {
		const ImVec2 text_pos = get_centered_tile_label_position(layout.Cursor, text);
		draw_list->AddText(text_pos, IM_COL32(230, 230, 230, 255), text.c_str());
	}

	static void show_full_text_tooltip_when_clipped(bool hovered, const std::string& displayed, const std::string& full_text) {
		if (!hovered || displayed == full_text)
			return;

		ImGui::BeginTooltip();
		ImGui::TextUnformatted(full_text.c_str());
		ImGui::EndTooltip();
	}

	static int calculate_tile_columns(float panel_width) {
		const float cell_size = k_tile_size + k_tile_padding;
		return std::max(1, (int)(panel_width / cell_size));
	}

	static const char* get_folder_name_or_root_label(const FolderEntry* folder) {
		return folder->Name.empty() ? "Assets" : folder->Name.c_str();
	}

	static float calculate_breadcrumb_total_width(const std::vector<FolderEntry*>& path) {
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

	static void align_breadcrumb_items_vertically(float item_height) {
		const float avail_y = ImGui::GetContentRegionAvail().y;
		float y_offset = 0.0f;
		if (avail_y > item_height)
			y_offset = (avail_y - item_height) * 0.5f;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y_offset);
	}

	static void push_breadcrumb_button_colors(bool is_last) {
		if (!is_last) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.06f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.10f));
			return;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.06f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.08f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.10f));
	}

	static void draw_breadcrumb_separator(ImU32 separator_color) {
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(separator_color));
		ImGui::TextUnformatted(">");
		ImGui::PopStyleColor();
		ImGui::SameLine();
	}

	static void render_breadcrumb_segment(
	    std::vector<FolderEntry*>& path,
	    FolderEntry*& current_folder,
	    size_t index,
	    float available_width,
	    ImU32 separator_color) {
		ImGui::PushID((int)index);

		const bool is_last = (index + 1 == path.size());
		const char* raw_label = get_folder_name_or_root_label(path[index]);
		const float max_segment_width = std::min(k_breadcrumb_max_segment_width, available_width * 0.45f);
		const std::string label = clip_text_to_width(raw_label, max_segment_width);

		push_breadcrumb_button_colors(is_last);
		if (ImGui::Button(label.c_str())) {
			path.resize(index + 1);
			current_folder = path.back();
		}
		ImGui::PopStyleColor(3);

		if (ImGui::IsItemHovered() && label != raw_label) {
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(raw_label);
			ImGui::EndTooltip();
		}

		ImGui::PopID();

		if (!is_last)
			draw_breadcrumb_separator(separator_color);
	}

	static ImU32 get_folder_tile_background_color(bool hovered) {
		return hovered ? IM_COL32(80, 80, 80, 140) : IM_COL32(50, 50, 50, 90);
	}

	static ImU32 get_asset_tile_background_color(bool is_selected, bool hovered) {
		return is_selected ? IM_COL32(60, 120, 200, 180)
		    : hovered      ? IM_COL32(80, 80, 80, 140)
		                   : IM_COL32(50, 50, 50, 90);
	}

	static void draw_folder_thumbnail(ImDrawList* draw_list, const TileLayout& layout) {
		ImTextureID icon = EditorIcons::get_instance().get(IconType::Folder);
		if (icon) {
			draw_list->AddImage(icon, layout.ThumbnailMin, layout.ThumbnailMax);
			return;
		}

		draw_placeholder_thumbnail(draw_list, layout);
	}

	static void add_image_rotated(ImDrawList* drawList, ImTextureID texture, ImVec2 center, ImVec2 size, float angleRad, ImU32 tint = IM_COL32_WHITE) {
		const ImVec2 half = size * 0.5f;
		const float c = cosf(angleRad);
		const float s = sinf(angleRad);

		auto rotate = [&](ImVec2 p) {
			p -= half;
			return ImVec2(
			    center.x + p.x * c - p.y * s,
			    center.y + p.x * s + p.y * c);
		};

		ImVec2 p0 = rotate({ 0, 0 });
		ImVec2 p1 = rotate({ size.x, 0 });
		ImVec2 p2 = rotate({ size.x, size.y });
		ImVec2 p3 = rotate({ 0, size.y });

		drawList->AddImageQuad(
		    texture,
		    p0, p1, p2, p3,
		    ImVec2(0, 0),
		    ImVec2(1, 0),
		    ImVec2(1, 1),
		    ImVec2(0, 1),
		    tint);
	}

	static void draw_asset_thumbnail(ImDrawList* draw_list, const TileLayout& layout, const AssetEntry& asset) {
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

	static std::filesystem::path make_unique_material_path(const std::filesystem::path& folder) {
		int i = 0;
		std::filesystem::path path;

		do {
			std::string name = i == 0 ? "material.mat"
			                          : "material_" + std::to_string(i) + ".mat";

			path = folder / name;
			i++;
		} while (std::filesystem::exists(path));

		return path;
	}

}

AssetPanel::AssetPanel(EditorContext& ctx)
    : m_context(ctx) {
	const auto& metadata_map = Origo::AssetDatabase::get_instance().get_all_metadata();

	std::vector<Origo::AssetMetadata> snapshot;
	snapshot.reserve(metadata_map.size());
	for (const auto& [id, meta] : metadata_map)
		snapshot.push_back(meta);

	m_tree.build(snapshot);
	m_current_folder = m_tree.get_root();
	if (m_current_folder)
		m_path = { m_current_folder };
}

void AssetPanel::on_im_gui_render() {
	ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoScrollbar);

	draw_top_bar();

	ImGui::Separator();

	ImGui::BeginChild("AssetBrowser", ImVec2(0, 0), false);
	draw_folder_contents(m_current_folder);
	ImGui::EndChild();

	ImGui::End();
}

void AssetPanel::draw_top_bar() {
	const float height = ImGui::GetFrameHeight() + ImGui::GetStyle().WindowPadding.y * 0.5f;
	ImGui::BeginChild("AssetTopBar", ImVec2(0, height), false, ImGuiWindowFlags_NoScrollbar);

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::SameLine();

	float avail = ImGui::GetContentRegionAvail().x;
	float breadcrumb_width = std::max(0.0f, avail - k_search_width - style.ItemSpacing.x);

	draw_breadcrumb_bar(breadcrumb_width);

	ImGui::SameLine();

	ImGui::SetNextItemWidth(k_search_width);
	ImGui::InputTextWithHint("##AssetSearch", "Search...", &m_search);

	ImGui::EndChild();
}

void AssetPanel::draw_breadcrumb_bar(float availableWidth) {
	if (!m_current_folder)
		return;

	const float h = ImGui::GetFrameHeight();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::BeginChild("Breadcrumb", ImVec2(availableWidth, h), false, flags);

	const float item_h = ImGui::GetFrameHeight();
	align_breadcrumb_items_vertically(item_h);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(k_breadcrumb_item_spacing_x, 0.0f));

	const ImU32 sep_color = ImGui::GetColorU32(ImGuiCol_TextDisabled);

	if (calculate_breadcrumb_total_width(m_path) > availableWidth)
		ImGui::SetScrollX(ImGui::GetScrollMaxX());

	for (size_t i = 0; i < m_path.size(); ++i)
		render_breadcrumb_segment(m_path, m_current_folder, i, availableWidth, sep_color);

	ImGui::PopStyleVar(1);
	ImGui::EndChild();
}

void AssetPanel::draw_folder_contents(FolderEntry* folder) {
	if (!folder)
		return;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	draw_create_asset_context_menu();

	const float panel_width = ImGui::GetContentRegionAvail().x;
	const int column_count = calculate_tile_columns(panel_width);

	ImGui::Columns(column_count, nullptr, false);

	for (FolderEntry* child : folder->Children) {
		if (!contains_case_insensitive(child->Name, m_search))
			continue;
		draw_folder_tile(child, draw_list);
		ImGui::NextColumn();
	}

	for (AssetEntry*& asset : folder->Assets) {
		if (!contains_case_insensitive(asset->Name, m_search))
			continue;
		draw_asset_tile(asset, draw_list);
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
}

void AssetPanel::draw_folder_tile(FolderEntry* folder, ImDrawList* drawList) {
	ImGui::PushID(folder);

	const TileLayout layout = make_tile_layout();
	ImGui::InvisibleButton("folder_tile", layout.TileSize);

	const bool hovered = ImGui::IsItemHovered();
	const bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
	const bool double_clicked = hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

	draw_tile_background(drawList, layout, get_folder_tile_background_color(hovered));
	draw_folder_thumbnail(drawList, layout);

	const std::string display_name = clip_text_to_width(folder->Name, get_tile_label_max_width());
	draw_tile_label(drawList, layout, display_name);
	show_full_text_tooltip_when_clipped(hovered, display_name, folder->Name);

	if (clicked || double_clicked) {
		m_current_folder = folder;
		m_path.push_back(folder);
	}

	ImGui::PopID();
}

void AssetPanel::draw_asset_tile(AssetEntry*& asset, ImDrawList* drawList) {
	ImGui::PushID(&asset->Id);

	const TileLayout layout = make_tile_layout();
	ImGui::InvisibleButton("asset_tile", layout.TileSize);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		m_context.set_selected_asset(asset->Id);

	const bool hovered = ImGui::IsItemHovered();

	const auto selected_asset_id = m_context.get_selected_asset();
	const bool is_selected = (asset->Id == selected_asset_id);

	draw_tile_background(drawList, layout, get_asset_tile_background_color(is_selected, hovered));
	draw_asset_thumbnail(drawList, layout, *asset);

	const std::string display_name = clip_text_to_width(asset->Name, get_tile_label_max_width());
	draw_tile_label(drawList, layout, display_name);
	show_full_text_tooltip_when_clipped(hovered, display_name, asset->Name);

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		const std::string uuid_str = (asset->Id).to_string();

		ImGui::TextUnformatted(asset->Name.c_str());
		ImGui::SetDragDropPayload(
		    "ORIGO_ASSET_UUID",
		    uuid_str.c_str(),
		    uuid_str.size() + 1);

		ImGui::EndDragDropSource();
	}

	ImGui::PopID();
}

void AssetPanel::draw_create_asset_context_menu() {
	using namespace Origo;

	if (!ImGui::BeginPopupContextWindow("AssetPanelContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		return;

	if (ImGui::BeginMenu("Create")) {
		if (ImGui::MenuItem("Material")) {
			if (!std::filesystem::exists(m_current_folder->Path)) {
				ORG_ERROR("Unable to create materials within a virtual path");
			}

			auto material_path = make_unique_material_path(m_current_folder->Path);
			Origo::JsonSerializer backend { material_path };
			auto handle { Origo::DefaultAssetCache::get_instance().get_material() };

			Origo::AssetSerializationSystem::
			    get(Origo::AssetType::Material2D)
			        ->serialize(Origo::AssetManager::get_instance().get(handle), backend);

			backend.save_to_file();
		}
		if (ImGui::MenuItem("Script")) {
			if (!std::filesystem::exists(m_current_folder->Path)) {
				ORG_ERROR("Unable to create shaders within a virtual path");
			}
			// TODO: Create a new script file in the current folder.
		}
		if (ImGui::MenuItem("Shader")) {
			if (!std::filesystem::exists(m_current_folder->Path)) {
				ORG_ERROR("Unable to create shaders within a virtual path");
			}
			// TODO: Create a new shader asset in the current folder.
		}
		ImGui::EndMenu();
	}

	ImGui::EndPopup();
}

}
