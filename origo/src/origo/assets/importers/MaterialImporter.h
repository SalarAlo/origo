#pragma once

#include "origo/assets/importers/IAssetImporter.h"
namespace Origo {

class MaterialImporter : public IAssetImporter {
public:
	bool can_import(const std::filesystem::path& path) const override;
	AssetType get_asset_type() const override;
	Scope<Asset> import(const std::filesystem::path& path, const AssetMetadata& meta) override;

private:
};

}
