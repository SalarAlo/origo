#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/Metadata.h"
#include <filesystem>

namespace Origo {

class IAssetImporter {
public:
	virtual ~IAssetImporter() = default;

	virtual bool can_import(const std::filesystem::path& path) const = 0;
	virtual AssetType get_asset_type() const = 0;
	// TODO: Metadata alr contains path refactor!
	virtual Scope<Asset> import(const std::filesystem::path& path, const AssetMetadata& meta) = 0;
};

}
