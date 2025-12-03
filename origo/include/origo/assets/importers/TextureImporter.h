#pragma once

#include "origo/assets/importers/IAssetImporter.h"
namespace Origo {

class TextureImporter final : public IAssetImporter {
public:
	bool CanImport(const std::filesystem::path& path) override;
	AssetType GetAssetType() override;
	void Import(const std::filesystem::path& path, const AssetMetadata& meta) override;

private:
};

}
