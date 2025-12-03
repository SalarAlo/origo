#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/AssetMetadata.h"
namespace Origo {

class IAssetImporter {
public:
	virtual ~IAssetImporter() = default;

	virtual bool CanImport(const std::filesystem::path& path) = 0;
	virtual AssetType GetAssetType() = 0;
	virtual void Import(const std::filesystem::path& path, const AssetMetadata& meta) = 0;

private:
};

}
