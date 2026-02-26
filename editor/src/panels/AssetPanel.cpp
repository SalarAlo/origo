#include <cctype>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <string>

#include "panels/AssetPanel.h"

#include "imgui_internal.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetDatabase.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

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

	float search_width = 220.0f;
	float avail = ImGui::GetContentRegionAvail().x;
	float breadcrumb_width = std::max(0.0f, avail - search_width - style.ItemSpacing.x);

	draw_breadcrumb_bar(breadcrumb_width);

	ImGui::SameLine();

	ImGui::SetNextItemWidth(search_width);
	ImGui::InputTextWithHint("##AssetSearch", "Search...", &m_search);

	ImGui::EndChild();
}

void AssetPanel::draw_breadcrumb_bar(float availableWidth) {
	if (!m_current_folder)
		return;

	const float h = ImGui::GetFrameHeight();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::BeginChild("Breadcrumb", ImVec2(availableWidth, h), false, flags);

	const float child_h = h;

	const float item_h = ImGui::GetFrameHeight();
	const float avail_y = ImGui::GetContentRegionAvail().y;

	float y_offset = 0.0f;
	if (avail_y > item_h)
		y_offset = (avail_y - item_h) * 0.5f;

	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y_offset);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 0.0f));

	const ImU32 sep_color = ImGui::GetColorU32(ImGuiCol_TextDisabled);

	float total_width = 0.0f;
	for (size_t i = 0; i < m_path.size(); ++i) {
		const char* label = m_path[i]->Name.empty() ? "Assets" : m_path[i]->Name.c_str();
		total_width += ImGui::CalcTextSize(label).x + ImGui::GetStyle().FramePadding.x * 2.0f;
		if (i + 1 < m_path.size())
			total_width += ImGui::CalcTextSize(">").x + ImGui::GetStyle().ItemSpacing.x * 2.0f;
	}

	if (total_width > availableWidth)
		ImGui::SetScrollX(ImGui::GetScrollMaxX());

	for (size_t i = 0; i < m_path.size(); ++i) {
		ImGui::PushID((int)i);

		const bool is_last = (i + 1 == m_path.size());
		const char* raw_label = m_path[i]->Name.empty() ? "Assets" : m_path[i]->Name.c_str();

		const float max_segment_width = std::min(220.0f, availableWidth * 0.45f);
		std::string label = clip_text_to_width(raw_label, max_segment_width);

		if (!is_last) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.06f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.10f));
		} else {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.06f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.08f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.10f));
		}

		if (ImGui::Button(label.c_str())) {
			m_path.resize(i + 1);
			m_current_folder = m_path.back();
		}

		ImGui::PopStyleColor(3);

		if (ImGui::IsItemHovered() && label != raw_label) {
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(raw_label);
			ImGui::EndTooltip();
		}

		ImGui::PopID();

		if (i + 1 < m_path.size()) {
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(sep_color));
			ImGui::TextUnformatted(">");
			ImGui::PopStyleColor();
			ImGui::SameLine();
		}
	}

	ImGui::PopStyleVar(1);
	ImGui::EndChild();
}

void AssetPanel::draw_folder_contents(FolderEntry* folder) {
	if (!folder)
		return;

	constexpr float tile_size = 96.0f;
	constexpr float tile_padding = 10.0f;
	constexpr float text_height = 22.0f;

	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	const float cell_size = tile_size + tile_padding;
	const float panel_width = ImGui::GetContentRegionAvail().x;
	const int column_count = std::max(1, (int)(panel_width / cell_size));

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
	constexpr float tile_size = 96.0f;
	constexpr float text_height = 22.0f;

	ImGui::PushID(folder);

	ImVec2 cursor = ImGui::GetCursorScreenPos();
	ImVec2 tile_size_vec = { tile_size, tile_size + text_height };

	ImGui::InvisibleButton("folder_tile", tile_size_vec);

	const bool hovered = ImGui::IsItemHovered();
	const bool clicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
	const bool double_clicked = hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

	ImU32 bg_color = hovered ? IM_COL32(80, 80, 80, 140) : IM_COL32(50, 50, 50, 90);
	drawList->AddRectFilled(cursor, cursor + tile_size_vec, bg_color, 8.0f);

	ImVec2 thumb_min = cursor + ImVec2(10, 10);
	ImVec2 thumb_max = thumb_min + ImVec2(tile_size - 20, tile_size - 20);

	ImTextureID icon = EditorIcons::get_instance().get(IconType::Folder);
	if (icon)
		drawList->AddImage(icon, thumb_min, thumb_max);
	else
		drawList->AddRectFilled(thumb_min, thumb_max, IM_COL32(90, 90, 90, 255), 6.0f);

	const float text_padding = 6.0f;
	const float max_width = tile_size - text_padding * 2.0f;

	std::string display_name = clip_text_to_width(folder->Name, max_width);
	ImVec2 text_size = ImGui::CalcTextSize(display_name.c_str());
	const float font_size = ImGui::GetFontSize();

	ImVec2 text_pos = {
		cursor.x + (tile_size - text_size.x) * 0.5f,
		cursor.y + tile_size + (text_height - font_size) * 0.5f
	};

	drawList->AddText(text_pos, IM_COL32(230, 230, 230, 255), display_name.c_str());

	if (hovered && display_name != folder->Name) {
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(folder->Name.c_str());
		ImGui::EndTooltip();
	}

	if (clicked || double_clicked) {
		m_current_folder = folder;
		m_path.push_back(folder);
	}

	ImGui::PopID();
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

void AssetPanel::draw_asset_tile(AssetEntry*& asset, ImDrawList* drawList) {
	constexpr float tile_size = 96.0f;
	constexpr float text_height = 22.0f;

	ImGui::PushID(&asset->Id);

	ImVec2 cursor = ImGui::GetCursorScreenPos();
	ImVec2 tile_size_vec = { tile_size, tile_size + text_height };

	ImGui::InvisibleButton("asset_tile", tile_size_vec);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		m_context.set_selected_asset(asset->Id);

	const bool hovered = ImGui::IsItemHovered();

	const auto selected_asset_id = m_context.get_selected_asset();
	const bool is_selected = (asset->Id == selected_asset_id);

	ImU32 bg_color = is_selected ? IM_COL32(60, 120, 200, 180)
	    : hovered                ? IM_COL32(80, 80, 80, 140)
	                             : IM_COL32(50, 50, 50, 90);

	drawList->AddRectFilled(cursor, cursor + tile_size_vec, bg_color, 8.0f);

	ImVec2 thumb_min = cursor + ImVec2(10, 10);
	ImVec2 thumb_max = thumb_min + ImVec2(tile_size - 20, tile_size - 20);

	if (asset->Icon) {
		ImVec2 size = thumb_max - thumb_min;
		ImVec2 center = thumb_min + size * 0.5f;

		if (asset->Type == Origo::AssetType::Texture2D) {
			add_image_rotated(drawList, asset->Icon, center, size, IM_PI);
		} else {
			add_image_rotated(drawList, asset->Icon, center, size, 0);
		}
	} else {
		drawList->AddRectFilled(thumb_min, thumb_max, IM_COL32(90, 90, 90, 255), 6.0f);
	}

	const float text_padding = 6.0f;
	const float max_width = tile_size - text_padding * 2.0f;

	std::string display_name = clip_text_to_width(asset->Name, max_width);
	ImVec2 text_size = ImGui::CalcTextSize(display_name.c_str());
	const float font_size = ImGui::GetFontSize();

	ImVec2 text_pos = {
		cursor.x + (tile_size - text_size.x) * 0.5f,
		cursor.y + tile_size + (text_height - font_size) * 0.5f
	};

	drawList->AddText(text_pos, IM_COL32(230, 230, 230, 255), display_name.c_str());

	if (hovered && display_name != asset->Name) {
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(asset->Name.c_str());
		ImGui::EndTooltip();
	}

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

}
