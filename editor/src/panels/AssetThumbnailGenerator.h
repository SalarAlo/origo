#pragma once

#include "panels/EditorAssetTree.h"

namespace OrigoEditor {

class AssetThumbnailGenerator {
public:
	static ImTextureID GetThumbnailID(const AssetEntry* entry);
};

}
