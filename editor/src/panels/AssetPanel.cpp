#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

#include "panels/AssetPanel.h"

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"

#include "systems/EditorIcons.h"

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

	bool hierarchy_node_matches_search(
	    const AssetEntry& asset,
	    const AssetBrowserState& browser_state,
	    const std::unordered_map<Origo::UUID, std::vector<AssetEntry*>>& children_by_parent) {
		if (browser_state.matches_search(asset.Name))
			return true;

		auto children_it = children_by_parent.find(asset.Id);
		if (children_it == children_by_parent.end())
			return false;

		for (AssetEntry* child : children_it->second) {
			if (child && hierarchy_node_matches_search(*child, browser_state, children_by_parent))
				return true;
		}

		return false;
	}

	void draw_tree_node_icon(ImTextureID icon) {
		if (!icon)
			return;

		const float icon_size = ImGui::GetFontSize() - 2.0f;
		const ImVec2 min = ImGui::GetItemRectMin();
		const ImVec2 max = ImGui::GetItemRectMax();
		const float spacing = ImGui::GetTreeNodeToLabelSpacing();
		const float row_h = max.y - min.y;
		const ImVec2 p_min {
			min.x + spacing + 2.0f,
			min.y + (row_h - icon_size) * 0.5f
		};
		const ImVec2 p_max { p_min.x + icon_size, p_min.y + icon_size };

		ImGui::GetWindowDrawList()->AddImage(icon, p_min, p_max);
	}

	bool should_toggle_row_open(bool has_children) {
		return has_children
		    && ImGui::IsItemClicked(ImGuiMouseButton_Left)
		    && !ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
		    && !ImGui::IsItemToggledOpen();
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

void AssetPanel::draw_folder_contents(FolderEntry* folder) {
	if (!folder)
		return;

	draw_create_asset_context_menu();
	draw_folder_tree_node(*folder);
}

void AssetPanel::draw_folder_tree_node(FolderEntry& folder) {
	std::unordered_map<Origo::UUID, AssetEntry*> by_id;
	by_id.reserve(folder.Assets.size());

	std::unordered_map<Origo::UUID, std::vector<AssetEntry*>> children_by_parent;
	children_by_parent.reserve(folder.Assets.size());

	for (AssetEntry* asset : folder.Assets) {
		if (!asset)
			continue;
		by_id.insert_or_assign(asset->Id, asset);
	}

	std::vector<AssetEntry*> root_assets;
	root_assets.reserve(folder.Assets.size());
	for (AssetEntry* asset : folder.Assets) {
		if (!asset)
			continue;

		if (!asset->ParentId || !by_id.contains(*asset->ParentId)) {
			root_assets.push_back(asset);
			continue;
		}

		children_by_parent[*asset->ParentId].push_back(asset);
	}

	auto sort_by_name = [](const auto* a, const auto* b) {
		return a->Name < b->Name;
	};
	std::sort(folder.Children.begin(), folder.Children.end(), sort_by_name);
	std::sort(root_assets.begin(), root_assets.end(), sort_by_name);
	for (auto& [parent, children] : children_by_parent)
		std::sort(children.begin(), children.end(), sort_by_name);

	std::function<bool(const FolderEntry&)> folder_or_descendant_matches = [&](const FolderEntry& folder_node) -> bool {
		if (m_browser_state.matches_search(folder_node.Name))
			return true;

		for (AssetEntry* asset : folder_node.Assets) {
			if (asset && m_browser_state.matches_search(asset->Name))
				return true;
		}

		for (FolderEntry* child : folder_node.Children) {
			if (child && folder_or_descendant_matches(*child))
				return true;
		}

		return false;
	};

	struct MixedNode {
		bool IsFolder { false };
		FolderEntry* Folder { nullptr };
		AssetEntry* Asset { nullptr };
		std::string Name {};
	};

	std::vector<MixedNode> mixed_nodes;
	mixed_nodes.reserve(folder.Children.size() + root_assets.size());
	for (FolderEntry* child : folder.Children) {
		if (!child)
			continue;
		mixed_nodes.push_back({ true, child, nullptr, child->Name });
	}
	for (AssetEntry* asset : root_assets) {
		if (!asset)
			continue;
		mixed_nodes.push_back({ false, nullptr, asset, asset->Name });
	}
	std::sort(mixed_nodes.begin(), mixed_nodes.end(), [](const MixedNode& a, const MixedNode& b) {
		return a.Name < b.Name;
	});

	for (const MixedNode& node : mixed_nodes) {
		if (node.IsFolder) {
			FolderEntry& child = *node.Folder;
			const bool has_children = !child.Children.empty() || !child.Assets.empty();
			if (!folder_or_descendant_matches(child))
				continue;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
			if (!has_children)
				flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			ImGui::PushID(&child);
			const std::string label = "      " + (child.Name.empty() ? std::string("Assets") : child.Name);
			const bool open = ImGui::TreeNodeEx("folder_node", flags, "%s", label.c_str());
			draw_tree_node_icon(EditorIcons::get_instance().get(IconType::Folder));
			if (should_toggle_row_open(has_children)) {
				const bool next_open_state = !open;
				ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), next_open_state ? 1 : 0);
			}

			if (has_children && open) {
				draw_folder_tree_node(child);
				ImGui::TreePop();
			}

			ImGui::PopID();
			continue;
		}

		draw_asset_hierarchy_node(*node.Asset, children_by_parent);
	}
}

void AssetPanel::draw_folder_tile(FolderEntry* folder, ImDrawList* draw_list) {
	ImGui::PushID(folder);

	const AssetTileLayout layout = AssetLayout::make_tile_layout();
	ImGui::InvisibleButton("folder_tile", layout.TileSize);

	m_interaction.handle_folder_tile(folder, m_browser_state, m_tree);
	m_tile_renderer.draw_folder_tile(draw_list, layout, *folder, ImGui::IsItemHovered());

	ImGui::PopID();
}

void AssetPanel::draw_asset_tile(AssetEntry& asset, ImDrawList* draw_list) {
	ImGui::PushID(&asset.Id);

	const AssetTileLayout layout = AssetLayout::make_tile_layout();
	ImGui::InvisibleButton("asset_tile", layout.TileSize);

	m_interaction.handle_asset_tile(asset, m_context);
	m_tile_renderer.draw_asset_tile(draw_list, layout, asset, ImGui::IsItemHovered(), m_interaction.is_asset_selected(asset, m_context));

	ImGui::PopID();
}

void AssetPanel::draw_asset_hierarchy(FolderEntry* folder) {
	if (!folder || folder->Assets.empty())
		return;

	std::unordered_map<Origo::UUID, AssetEntry*> by_id;
	by_id.reserve(folder->Assets.size());

	std::unordered_map<Origo::UUID, std::vector<AssetEntry*>> children_by_parent;
	children_by_parent.reserve(folder->Assets.size());

	for (AssetEntry* asset : folder->Assets) {
		if (!asset)
			continue;
		by_id[asset->Id] = asset;
	}

	std::vector<AssetEntry*> roots;
	roots.reserve(folder->Assets.size());

	for (AssetEntry* asset : folder->Assets) {
		if (!asset)
			continue;

		if (!asset->ParentId || !by_id.contains(*asset->ParentId)) {
			roots.push_back(asset);
			continue;
		}

		children_by_parent[*asset->ParentId].push_back(asset);
	}

	auto sort_by_name = [](AssetEntry* a, AssetEntry* b) {
		return a->Name < b->Name;
	};
	std::sort(roots.begin(), roots.end(), sort_by_name);
	for (auto& [parent, children] : children_by_parent)
		std::sort(children.begin(), children.end(), sort_by_name);

	ImGui::Separator();
	ImGui::TextDisabled("Assets");

	for (AssetEntry* root : roots) {
		if (!root || !hierarchy_node_matches_search(*root, m_browser_state, children_by_parent))
			continue;
		draw_asset_hierarchy_node(*root, children_by_parent);
	}
}

void AssetPanel::draw_asset_hierarchy_node(
    AssetEntry& asset,
    const std::unordered_map<Origo::UUID, std::vector<AssetEntry*>>& children_by_parent) {
	const auto children_it = children_by_parent.find(asset.Id);
	const bool has_children = children_it != children_by_parent.end() && !children_it->second.empty();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
	if (!has_children)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	const auto selected_asset_id = m_context.get_selected_asset();
	if (selected_asset_id && *selected_asset_id == asset.Id)
		flags |= ImGuiTreeNodeFlags_Selected;

	const std::string node_id = asset.Id.to_string();
	ImGui::PushID(node_id.c_str());
	std::string label = "      " + asset.Name;
	if (has_children)
		label += " (" + std::to_string(children_it->second.size()) + ")";
	const bool open = ImGui::TreeNodeEx("asset_node", flags, "%s", label.c_str());
	draw_tree_node_icon(asset.Icon);
	if (should_toggle_row_open(has_children)) {
		const bool next_open_state = !open;
		ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), next_open_state ? 1 : 0);
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		m_context.set_selected_asset(asset.Id);

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		const std::string uuid = asset.Id.to_string();
		ImGui::TextUnformatted(asset.Name.c_str());
		ImGui::SetDragDropPayload("ORIGO_ASSET_UUID", uuid.c_str(), uuid.size() + 1);
		ImGui::EndDragDropSource();
	}

	if (has_children && open) {
		for (AssetEntry* child : children_it->second) {
			if (!child || !hierarchy_node_matches_search(*child, m_browser_state, children_by_parent))
				continue;
			draw_asset_hierarchy_node(*child, children_by_parent);
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void AssetPanel::draw_create_asset_context_menu() {
	if (!ImGui::BeginPopupContextWindow("AssetPanelContext", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		return;

	if (ImGui::BeginMenu("Create")) {
		if (ImGui::MenuItem("Material") && m_creation_service.create_material(m_browser_state.get_current_folder()))
			m_tree_dirty = true;

		if (ImGui::MenuItem("Script") && m_creation_service.create_script(m_browser_state.get_current_folder()))
			m_tree_dirty = true;

		if (ImGui::MenuItem("Shader") && m_creation_service.create_shader(m_browser_state.get_current_folder()))
			m_tree_dirty = true;

		ImGui::EndMenu();
	}

	ImGui::EndPopup();
}

}
