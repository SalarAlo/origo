#include "origo/assets/importers/AssetImporterRegistry.h"

#include "origo/assets/importers/IAssetImporter.h"
#include "origo/assets/importers/MaterialImporter.h"
#include "origo/assets/importers/ModelImporter.h"
#include "origo/assets/importers/SceneAssetImporter.h"
#include "origo/assets/importers/ScriptImporter.h"
#include "origo/assets/importers/ShaderImporter.h"
#include "origo/assets/importers/TextureImporter.h"

namespace Origo {
void AssetImporterRegistry::Register(Scope<IAssetImporter> importer) {
	m_importers.emplace_back(std::move(importer));
}

IAssetImporter* AssetImporterRegistry::get_importer(const std::filesystem::path& file) {
	for (const auto& importer : m_importers) {
		if (importer->can_import(file))
			return importer.get();
	}

	return nullptr;
}

void AssetImporterRegistry::initialise_default_importers() {
	ORG_CORE_TRACE("Registering all default asset importers");
	Register(MakeScope<TextureImporter>());
	Register(MakeScope<ShaderImporter>());
	Register(MakeScope<ScriptImporter>());
	Register(MakeScope<ModelImporter>());
	Register(MakeScope<MaterialImporter>());
	Register(MakeScope<SceneAssetImporter>());
}

}
