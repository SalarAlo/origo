#pragma once

#include "origo/assets/importers/IAssetImporter.h"

#include "origo/utils/Singleton.h"

namespace Origo {
class AssetImporterRegistry : public Singleton<AssetImporterRegistry> {
public:
	void Register(Scope<IAssetImporter> importer);
	IAssetImporter* GetImporter(const std::filesystem::path& file);
	void InitialiseDefaultImporters();

private:
	std::vector<Scope<IAssetImporter>> m_Importers {};
};

}
