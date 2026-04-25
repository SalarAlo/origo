#pragma once

#include "origo/assets/importers/IAssetImporter.h"

#include "origo/utils/Singleton.h"

namespace Origo {
class AssetImporterRegistry : public Singleton<AssetImporterRegistry> {
public:
	void register_importer(Scope<IAssetImporter> importer);
	IAssetImporter* get_importer(const std::filesystem::path& file);
	void initialise_default_importers();

private:
	std::vector<Scope<IAssetImporter>> m_importers {};
};

}
