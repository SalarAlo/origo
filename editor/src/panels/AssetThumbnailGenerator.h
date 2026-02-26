#pragma once

#include "panels/EditorAssetTree.h"

namespace OrigoEditor {

class AssetThumbnailGenerator {
public:
	static ImTextureID get_thumbnail_id(const AssetEntry* entry);
};

}
