#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/Metadata.h"
#include <filesystem>

namespace Origo {

class IAssetImporter {
public:
	virtual ~IAssetImporter() = default;

	virtual bool CanImport(const std::filesystem::path& path) const = 0;
	virtual AssetType GetAssetType() const = 0;
	virtual Scope<Asset> Import(const std::filesystem::path& path, const AssetMetadata& meta) = 0;
};

}
