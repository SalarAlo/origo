#include <imgui.h>

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "panels/AssetPanel.h"
#include "panels/AssetThumbnailGenerator.h"

#include "systems/EditorIcons.h"

namespace OrigoEditor {

namespace {
	struct AssetHierarchyData {
		std::unordered_map<Origo::UUID, AssetEntry*> ById;
		std::unordered_map<Origo::UUID, std::vector<AssetEntry*>> ChildrenByParent;
		std::vector<AssetEntry*> Roots;
	};

	struct MixedNode {
		bool IsFolder { false };
		FolderEntry* Folder { nullptr };
		AssetEntry* Asset { nullptr };
		std::string Name {};
	};

	struct BrowserEntries {
		std::vector<FolderEntry*> Folders;
		std::vector<AssetEntry*> Assets;
	};

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

	AssetHierarchyData build_asset_hierarchy_data(const std::vector<AssetEntry*>& assets) {
		AssetHierarchyData hierarchy {};
		hierarchy.ById.reserve(assets.size());
		hierarchy.ChildrenByParent.reserve(assets.size());
		hierarchy.Roots.reserve(assets.size());

		for (AssetEntry* asset : assets) {
			if (!asset)
				continue;
			hierarchy.ById.insert_or_assign(asset->Id, asset);
		}

		for (AssetEntry* asset : assets) {
			if (!asset)
				continue;

			if (!asset->ParentId || !hierarchy.ById.contains(*asset->ParentId)) {
				hierarchy.Roots.push_back(asset);
				continue;
			}

			hierarchy.ChildrenByParent[*asset->ParentId].push_back(asset);
		}

		return hierarchy;
	}

	void sort_folder_children(FolderEntry& folder) {
		auto sort_by_name = [](const auto* a, const auto* b) {
			return a->Name < b->Name;
		};

		std::sort(folder.Children.begin(), folder.Children.end(), sort_by_name);
	}

	void sort_asset_hierarchy_data(AssetHierarchyData& hierarchy) {
		auto sort_by_name = [](const auto* a, const auto* b) {
			return a->Name < b->Name;
		};

		std::sort(hierarchy.Roots.begin(), hierarchy.Roots.end(), sort_by_name);
		for (auto& [parent, children] : hierarchy.ChildrenByParent)
			std::sort(children.begin(), children.end(), sort_by_name);
	}

	std::vector<MixedNode> build_mixed_nodes(
	    const FolderEntry& folder,
	    const std::vector<AssetEntry*>& root_assets) {
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

		return mixed_nodes;
	}

	bool folder_or_descendant_matches_search(
	    const FolderEntry& folder,
	    const AssetBrowserState& browser_state) {
		if (browser_state.matches_search(folder.Name))
			return true;

		for (AssetEntry* asset : folder.Assets) {
			if (asset && browser_state.matches_search(asset->Name))
				return true;
		}

		for (FolderEntry* child : folder.Children) {
			if (child && folder_or_descendant_matches_search(*child, browser_state))
				return true;
		}

		return false;
	}

	BrowserEntries collect_browser_entries(FolderEntry& folder, const AssetBrowserState& browser_state) {
		BrowserEntries entries {};

		for (FolderEntry* child : folder.Children) {
			if (!child)
				continue;
			if (!browser_state.matches_search(child->Name))
				continue;
			entries.Folders.push_back(child);
		}

		for (AssetEntry* asset : folder.Assets) {
			if (!asset)
				continue;
			if (!browser_state.matches_search(asset->Name))
				continue;
			entries.Assets.push_back(asset);
		}

		auto sort_by_name = [](const auto* a, const auto* b) {
			return a->Name < b->Name;
		};

		std::sort(entries.Folders.begin(), entries.Folders.end(), sort_by_name);
		std::sort(entries.Assets.begin(), entries.Assets.end(), sort_by_name);
		return entries;
	}
}

void AssetPanel::draw_folder_contents(FolderEntry* folder) {
	if (!folder)
		return;

	if (m_view_mode == AssetPanelViewMode::Tree) {
		draw_folder_tree_node(*folder);
		return;
	}

	draw_browser_contents(m_browser_state.get_current_folder() ? m_browser_state.get_current_folder() : folder);
}

void AssetPanel::draw_browser_contents(FolderEntry* folder) {
	if (!folder)
		return;

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)
	    && !ImGui::IsAnyItemHovered()
	    && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup("AssetPanelCreatePopup");
	}

	draw_create_asset_context_menu(folder);
	handle_drag_drop_target(folder);

	if (m_browser_visual_mode == AssetBrowserVisualMode::List)
		draw_browser_list(*folder);
	else
		draw_browser_grid(*folder);
}

void AssetPanel::draw_browser_grid(FolderEntry& folder) {
	const BrowserEntries entries = collect_browser_entries(folder, m_browser_state);
	const bool has_entries = !entries.Folders.empty() || !entries.Assets.empty();

	if (!has_entries) {
		ImGui::TextDisabled("This folder is empty.");
		return;
	}

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const float available_width = ImGui::GetContentRegionAvail().x;
	const int columns = AssetLayout::calculate_tile_columns(available_width, m_browser_tile_size);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f, 12.0f));
	int index = 0;

	auto draw_entry = [&](auto* entry, auto draw_fn) {
		draw_fn(entry, draw_list, m_browser_tile_size);
		++index;
		if (index % columns != 0)
			ImGui::SameLine();
	};

	for (FolderEntry* child : entries.Folders)
		draw_entry(child, [this](FolderEntry* item, ImDrawList* list, float tile_size) { draw_folder_tile(item, list, tile_size); });

	for (AssetEntry* asset : entries.Assets)
		draw_entry(asset, [this](AssetEntry* item, ImDrawList* list, float tile_size) { draw_asset_tile(*item, list, tile_size); });

	ImGui::PopStyleVar();
}

void AssetPanel::draw_browser_list(FolderEntry& folder) {
	const BrowserEntries entries = collect_browser_entries(folder, m_browser_state);
	const bool has_entries = !entries.Folders.empty() || !entries.Assets.empty();

	if (!has_entries) {
		ImGui::TextDisabled("This folder is empty.");
		return;
	}

	const float row_height = 28.0f;
	const float icon_size = 18.0f;

	if (!ImGui::BeginTable("AssetBrowserList", 1, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInnerH))
		return;

	auto draw_folder_row = [&](FolderEntry& child) {
		ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
		ImGui::TableNextColumn();
		ImGui::PushID(&child);

		const bool clicked = ImGui::Selectable("##folder_row", false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, ImVec2(0.0f, row_height));
		handle_drag_drop_target(&child);

		const ImVec2 min = ImGui::GetItemRectMin();
		const float text_y = min.y + (row_height - ImGui::GetTextLineHeight()) * 0.5f;
		const float icon_y = min.y + (row_height - icon_size) * 0.5f;
		const float icon_x = min.x + 8.0f;

		if (ImTextureID icon = EditorIcons::get_instance().get(IconType::Folder))
			ImGui::GetWindowDrawList()->AddImage(icon, ImVec2(icon_x, icon_y), ImVec2(icon_x + icon_size, icon_y + icon_size));

		ImGui::GetWindowDrawList()->AddText(ImVec2(icon_x + icon_size + 10.0f, text_y), ImGui::GetColorU32(ImGuiCol_Text), child.Name.c_str());

		if (clicked)
			m_browser_state.navigate_to(&child, m_tree);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup("AssetPanelCreatePopup");

		draw_create_asset_context_menu(&child);
		ImGui::PopID();
	};

	auto draw_asset_row = [&](AssetEntry& asset) {
		ImGui::TableNextRow(ImGuiTableRowFlags_None, row_height);
		ImGui::TableNextColumn();
		ImGui::PushID(asset.Id.to_string().c_str());

		const bool selected = m_interaction.is_asset_selected(asset, m_context);
		ImGui::Selectable("##asset_row", selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap, ImVec2(0.0f, row_height));

		const ImVec2 min = ImGui::GetItemRectMin();
		const float text_y = min.y + (row_height - ImGui::GetTextLineHeight()) * 0.5f;
		const float icon_y = min.y + (row_height - icon_size) * 0.5f;
		const float icon_x = min.x + 8.0f;

		if (ImTextureID icon = AssetThumbnailGenerator::get_thumbnail_id(&asset))
			ImGui::GetWindowDrawList()->AddImage(icon, ImVec2(icon_x, icon_y), ImVec2(icon_x + icon_size, icon_y + icon_size), ImVec2(0, asset.Type == Origo::AssetType::Texture2D ? 1 : 0), ImVec2(1, asset.Type == Origo::AssetType::Texture2D ? 0 : 1));

		ImGui::GetWindowDrawList()->AddText(ImVec2(icon_x + icon_size + 10.0f, text_y), ImGui::GetColorU32(ImGuiCol_Text), asset.Name.c_str());

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			m_context.set_selected_asset(asset.Id);

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			const std::string uuid = asset.Id.to_string();
			ImGui::TextUnformatted(asset.Name.c_str());
			ImGui::SetDragDropPayload("ORIGO_ASSET_UUID", uuid.c_str(), uuid.size() + 1);
			ImGui::EndDragDropSource();
		}

		ImGui::PopID();
	};

	for (FolderEntry* child : entries.Folders)
		draw_folder_row(*child);

	for (AssetEntry* asset : entries.Assets)
		draw_asset_row(*asset);

	ImGui::EndTable();
}

void AssetPanel::draw_folder_tree_node(FolderEntry& folder) {
	AssetHierarchyData hierarchy = build_asset_hierarchy_data(folder.Assets);
	sort_folder_children(folder);
	sort_asset_hierarchy_data(hierarchy);
	std::vector<MixedNode> mixed_nodes = build_mixed_nodes(folder, hierarchy.Roots);

	for (const MixedNode& node : mixed_nodes) {
		if (node.IsFolder) {
			FolderEntry& child = *node.Folder;
			const bool has_children = !child.Children.empty() || !child.Assets.empty();
			if (!folder_or_descendant_matches_search(child, m_browser_state))
				continue;

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;
			if (!has_children)
				flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

			ImGui::PushID(&child);
			const std::string label = "      " + (child.Name.empty() ? std::string("Assets") : child.Name);
			const bool open = ImGui::TreeNodeEx("folder_node", flags, "%s", label.c_str());

			handle_drag_drop_target(&child);

			draw_tree_node_icon(EditorIcons::get_instance().get(IconType::Folder));
			if (should_toggle_row_open(has_children)) {
				const bool next_open_state = !open;
				ImGui::GetStateStorage()->SetInt(ImGui::GetItemID(), next_open_state ? 1 : 0);
			}
			const bool left_click_open = ImGui::IsItemClicked(ImGuiMouseButton_Left)
			    && !ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)
			    && !ImGui::IsItemToggledOpen();
			const bool right_click_open = ImGui::IsItemClicked(ImGuiMouseButton_Right);
			if (left_click_open || right_click_open)
				ImGui::OpenPopup("AssetPanelCreatePopup");

			draw_create_asset_context_menu(&child);

			if (has_children && open) {
				draw_folder_tree_node(child);
				ImGui::TreePop();
			}

			ImGui::PopID();
			continue;
		}

		draw_asset_hierarchy_node(*node.Asset, hierarchy.ChildrenByParent);
	}
}

void AssetPanel::draw_folder_tile(FolderEntry* folder, ImDrawList* draw_list, float tile_size) {
	ImGui::PushID(folder);

	const AssetTileLayout layout = AssetLayout::make_tile_layout(tile_size);
	ImGui::InvisibleButton("folder_tile", layout.TileSize);

	m_interaction.handle_folder_tile(folder, m_browser_state, m_tree);
	handle_drag_drop_target(folder);
	const bool left_click_open = ImGui::IsItemClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
	const bool right_click_open = ImGui::IsItemClicked(ImGuiMouseButton_Right);
	if (left_click_open || right_click_open)
		ImGui::OpenPopup("AssetPanelCreatePopup");
	draw_create_asset_context_menu(folder);
	m_tile_renderer.draw_folder_tile(draw_list, layout, *folder, ImGui::IsItemHovered());

	ImGui::PopID();
}

void AssetPanel::draw_asset_tile(AssetEntry& asset, ImDrawList* draw_list, float tile_size) {
	ImGui::PushID(asset.Id.to_string().c_str());

	const AssetTileLayout layout = AssetLayout::make_tile_layout(tile_size);
	ImGui::InvisibleButton("asset_tile", layout.TileSize);

	m_interaction.handle_asset_tile(asset, m_context);
	m_tile_renderer.draw_asset_tile(draw_list, layout, asset, ImGui::IsItemHovered(), m_interaction.is_asset_selected(asset, m_context));

	ImGui::PopID();
}

void AssetPanel::draw_asset_hierarchy(FolderEntry* folder) {
	if (!folder || folder->Assets.empty())
		return;

	AssetHierarchyData hierarchy = build_asset_hierarchy_data(folder->Assets);
	sort_asset_hierarchy_data(hierarchy);

	ImGui::Separator();
	ImGui::TextDisabled("Assets");

	for (AssetEntry* root : hierarchy.Roots) {
		if (!root || !hierarchy_node_matches_search(*root, m_browser_state, hierarchy.ChildrenByParent))
			continue;
		draw_asset_hierarchy_node(*root, hierarchy.ChildrenByParent);
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

void AssetPanel::handle_drag_drop_target(FolderEntry* entry) {
	if (!ImGui::BeginDragDropTarget())
		return;

	const ImGuiPayload* payload {};

	if (payload = ImGui::AcceptDragDropPayload("ORIGO_ENTITY_ID"); !payload) {
		ImGui::EndDragDropTarget();
		return;
	}

	const char* rid_str = static_cast<const char*>(payload->Data);
	Origo::RID ent = Origo::RID::from_string(rid_str);

	if (m_creation_service.create_prefab(entry, ent, m_context.ActiveScene))
		m_tree_dirty = true;

	ImGui::EndDragDropTarget();
}

}
