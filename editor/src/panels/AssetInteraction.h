#pragma once

#include "state/EditorContext.h"

namespace OrigoEditor {

class AssetBrowserState;
class EditorAssetTree;
struct AssetEntry;
struct FolderEntry;

class AssetInteraction {
public:
	void handle_folder_tile(FolderEntry* folder, AssetBrowserState& state, const EditorAssetTree& tree) const;
	void handle_asset_tile(const AssetEntry& asset, EditorContext& ctx) const;
	bool is_asset_selected(const AssetEntry& asset, const EditorContext& context) const;
};

}
