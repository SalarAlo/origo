#pragma once

#include <filesystem>

#include "origo/assets/Asset.h"
#include "origo/assets/Metadata.h"

#include "origo/assets/importers/AssetTruthLocation.h"

namespace Origo {

class IAssetImporter {
public:
	virtual ~IAssetImporter() = default;

	virtual bool can_import(const std::filesystem::path& path) const = 0;
	virtual AssetType get_asset_type() const = 0;
	// TODO: Metadata alr contains path refactor!
	virtual Scope<Asset> import(const std::filesystem::path& path, const AssetMetadata& meta) = 0;

	virtual AssetTruthLocation get_truth_location() const { return AssetTruthLocation::ImportPayload; }
	virtual AssetOrigin get_origin() const { return AssetOrigin::Imported; }
};

}
