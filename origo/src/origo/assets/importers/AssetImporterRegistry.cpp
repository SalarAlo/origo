#include "origo/assets/importers/AssetImporterRegistry.h"
#include "origo/assets/importers/IAssetImporter.h"
#include "origo/assets/importers/ShaderImporter.h"
#include "origo/assets/importers/TextureImporter.h"

namespace Origo {
void AssetImporterRegistry::Register(Scope<IAssetImporter> importer) {
	s_Importers.emplace_back(std::move(importer));
}

IAssetImporter* AssetImporterRegistry::GetImporter(const std::filesystem::path& file) {
	for (const auto& importer : s_Importers) {
		if (importer->CanImport(file))
			return importer.get();
	}

	return nullptr;
}

void AssetImporterRegistry::InitialiseDefaultImporters() {
	ORG_INFO("Registering all default asset importers");
	Register(MakeScope<TextureImporter>());
	Register(MakeScope<ShaderImporter>());
}

}
