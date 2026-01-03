#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/Metadata.h"

namespace OrigoEditor {

class AssetThumbnailCache {
public:
	ImTextureID GetThumbnail(Origo::AssetType type);

private:
	AssetThumbnailCache() = default;

	struct ThumbnailEntry {
		ImTextureID Texture = -1;
		bool Ready = false;
	};

private:
	ImTextureID GetFallbackIcon(Origo::AssetType type);
	ImTextureID GenerateThumbnail(const Origo::AssetMetadata& metadata);
};

}
