#pragma once

#include <string>
#include <vector>

#include "EditorPanel.h"

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
	void draw_breadcrumb_bar(float availableWidth);
	void draw_folder_contents(FolderEntry* folder);
	void draw_folder_tile(FolderEntry* folder, ImDrawList* drawList);
	void draw_asset_tile(AssetEntry*& asset, ImDrawList* drawList);

private:
	EditorContext& m_context;
	EditorAssetTree m_tree;

	FolderEntry* m_current_folder = nullptr;
	std::vector<FolderEntry*> m_path;

	std::string m_search;
};

}
