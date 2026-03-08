#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "EditorPanel.h"

#include "panels/AssetBrowserState.h"
#include "panels/AssetCreationService.h"
#include "panels/AssetInteraction.h"
#include "panels/AssetTileRenderer.h"
#include "panels/EditorAssetTree.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class AssetPanel : public EditorPanel {
public:
	AssetPanel(EditorContext& ctx);
	const char* get_name() const override { return "Assets"; }
	void on_im_gui_render() override;

private:
	void draw_top_bar();
	void draw_breadcrumb_bar(float available_width);
	void draw_folder_contents(FolderEntry* folder);
	void draw_folder_tree_node(FolderEntry& folder);
	void draw_folder_tile(FolderEntry* folder, ImDrawList* draw_list);
	void draw_asset_tile(AssetEntry& asset, ImDrawList* draw_list);
	void draw_asset_hierarchy(FolderEntry* folder);
	void draw_asset_hierarchy_node(AssetEntry& asset, const std::unordered_map<Origo::UUID, std::vector<AssetEntry*>>& children_by_parent);
	void draw_create_asset_context_menu(FolderEntry* target_folder);
	void rebuild_tree();

private:
	EditorContext& m_context;
	EditorAssetTree m_tree;

	AssetBrowserState m_browser_state;
	AssetTileRenderer m_tile_renderer;
	AssetInteraction m_interaction;
	AssetCreationService m_creation_service;

	bool m_tree_dirty { false };
};

}
