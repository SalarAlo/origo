#pragma once

#include <vector>

#include <imgui.h>

#include "panels/EditorAssetTree.h"

namespace OrigoEditor {

struct AssetTileLayout {
	ImVec2 Cursor;
	ImVec2 TileSize;
	ImVec2 ThumbnailMin;
	ImVec2 ThumbnailMax;
};

class AssetLayout {
public:
	static AssetTileLayout make_tile_layout(float tile_size);
	static int calculate_tile_columns(float panel_width, float tile_size);
	static float calculate_breadcrumb_total_width(const std::vector<FolderEntry*>& path);
	static float get_search_width();
	static float get_breadcrumb_item_spacing_x();
	static float get_breadcrumb_max_segment_width();
	static float get_tile_label_max_width(float tile_size);
};

}
