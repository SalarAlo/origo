#pragma once

#include "origo/assets/importers/IAssetImporter.h"

namespace Origo {
class AssetImporterRegistry {
public:
	static void Register(Scope<IAssetImporter> importer);
	static IAssetImporter* GetImporter(const std::filesystem::path& file);
	static void InitialiseDefaultImporters();

private:
	inline static std::vector<Scope<IAssetImporter>> s_Importers {};
};

}
