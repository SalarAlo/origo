#pragma once

#include "origo/assets/importers/IAssetImporter.h"
namespace Origo {

class ShaderImporter : public IAssetImporter {
public:
	bool CanImport(const std::filesystem::path& path) const override;
	AssetType GetAssetType() const override;
	Scope<Asset> Import(const std::filesystem::path& path, const AssetMetadata& meta) override;

private:
};

}
