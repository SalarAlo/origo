#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>

#include "panels/AssetPanel.h"

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"

namespace OrigoEditor {

namespace {
	const char* get_folder_name_or_root_label(const FolderEntry* folder) {
		return folder->Name.empty() ? "Assets" : folder->Name.c_str();
	}

	void align_breadcrumb_items_vertically(float item_height) {
		const float available_height = ImGui::GetContentRegionAvail().y;
		float y_offset = 0.0f;
		if (available_height > item_height)
			y_offset = (available_height - item_height) * 0.5f;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y_offset);
	}

	void push_breadcrumb_button_colors(bool is_last) {
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

	void draw_breadcrumb_separator(ImU32 separator_color) {
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(separator_color));
		ImGui::TextUnformatted(">");
		ImGui::PopStyleColor();
		ImGui::SameLine();
	}
}

AssetPanel::AssetPanel(EditorContext& ctx)
    : m_context(ctx) {
	rebuild_tree();
}

void AssetPanel::rebuild_tree() {
	const auto previous_path = m_browser_state.get_current_folder()
	    ? m_browser_state.get_current_folder()->Path
	    : std::filesystem::path {};

	const auto& metadata_map = Origo::AssetDatabase::get_instance().get_all_metadata();

	std::vector<Origo::AssetMetadata> snapshot;
	snapshot.reserve(metadata_map.size());
	for (const auto& [id, meta] : metadata_map)
		snapshot.push_back(meta);

	auto& am = Origo::AssetManager::get_instance();
	std::unordered_map<Origo::Asset*, Origo::UUID> pointer_to_uuid {};
	pointer_to_uuid.reserve(am.get_uuid_map().size());
	for (const auto& [uuid, handle] : am.get_uuid_map()) {
		if (auto* asset = am.get(handle))
			pointer_to_uuid.insert_or_assign(asset, uuid);
	}

	for (auto& md : snapshot) {
		if (md.ParentID || !md.ID)
			continue;

		auto handle = am.get_handle_by_uuid(*md.ID);
		if (!handle)
			continue;

		auto* asset = am.get(*handle);
		if (!asset || !asset->has_owner())
			continue;

		auto parent_it = pointer_to_uuid.find(asset->get_owner());
		if (parent_it != pointer_to_uuid.end())
			md.ParentID = parent_it->second;
	}

	m_tree.build(snapshot);
	m_browser_state.restore(m_tree, previous_path);
}

void AssetPanel::on_im_gui_render() {
	if (m_tree_dirty) {
		rebuild_tree();
		m_tree_dirty = false;
	}

	ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoScrollbar);

	draw_top_bar();

	ImGui::Separator();

	ImGui::BeginChild("AssetBrowser", ImVec2(0, 0), false);
	draw_folder_contents(m_tree.get_root());
	ImGui::EndChild();

	ImGui::End();
}

void AssetPanel::draw_top_bar() {
	const float height = ImGui::GetFrameHeight() + ImGui::GetStyle().WindowPadding.y * 0.5f;

	ImGui::BeginChild("AssetTopBar", ImVec2(0, height), false, ImGuiWindowFlags_NoScrollbar);

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::SameLine();

	const float available_width = ImGui::GetContentRegionAvail().x;
	const float breadcrumb_width = std::max(0.0f, available_width - AssetLayout::get_search_width() - style.ItemSpacing.x);

	draw_breadcrumb_bar(breadcrumb_width);

	ImGui::SameLine();

	ImGui::SetNextItemWidth(AssetLayout::get_search_width());
	ImGui::InputTextWithHint("##AssetSearch", "Search...", &m_browser_state.search());

	ImGui::EndChild();
}

void AssetPanel::draw_breadcrumb_bar(float available_width) {
	if (!m_browser_state.get_current_folder())
		return;

	const float height = ImGui::GetFrameHeight();
	const ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_HorizontalScrollbar;

	ImGui::BeginChild("Breadcrumb", ImVec2(available_width, height), false, flags);

	align_breadcrumb_items_vertically(ImGui::GetFrameHeight());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(AssetLayout::get_breadcrumb_item_spacing_x(), 0.0f));

	const ImU32 separator_color = ImGui::GetColorU32(ImGuiCol_TextDisabled);
	const auto& path = m_browser_state.get_path();

	if (AssetLayout::calculate_breadcrumb_total_width(path) > available_width)
		ImGui::SetScrollX(ImGui::GetScrollMaxX());

	for (size_t i = 0; i < path.size(); ++i) {
		ImGui::PushID((int)i);

		const bool is_last = (i + 1 == path.size());
		const char* raw_label = get_folder_name_or_root_label(path[i]);
		const float max_segment_width = std::min(AssetLayout::get_breadcrumb_max_segment_width(), available_width * 0.45f);
		const std::string label = m_tile_renderer.clip_label(raw_label, max_segment_width);

		push_breadcrumb_button_colors(is_last);
		if (ImGui::Button(label.c_str()))
			m_browser_state.navigate_to_breadcrumb(i);
		ImGui::PopStyleColor(3);

		m_tile_renderer.show_clipped_text_tooltip(ImGui::IsItemHovered(), label, raw_label);

		ImGui::PopID();

		if (!is_last)
			draw_breadcrumb_separator(separator_color);
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void AssetPanel::draw_create_asset_context_menu(FolderEntry* target_folder) {
	if (!ImGui::BeginPopup("AssetPanelCreatePopup"))
		return;

	if (ImGui::BeginMenu("Create")) {
		if (ImGui::MenuItem("Material") && m_creation_service.create_material(target_folder))
			m_tree_dirty = true;

		if (ImGui::MenuItem("Script") && m_creation_service.create_script(target_folder))
			m_tree_dirty = true;

		if (ImGui::MenuItem("Shader") && m_creation_service.create_shader(target_folder))
			m_tree_dirty = true;

		ImGui::EndMenu();
	}

	ImGui::EndPopup();
}

}
