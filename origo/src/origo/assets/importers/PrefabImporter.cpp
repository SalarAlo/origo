
#include "PrefabImporter.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/Prefab.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/core/Typedefs.h"

#include "origo/renderer/Helpers.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {

bool PrefabImporter::can_import(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	to_lower_in_place(ext);
	return ext == ".prefab";
}

AssetType PrefabImporter::get_asset_type() const {
	return AssetType::Prefab;
}

Scope<Asset> PrefabImporter::import(const std::filesystem::path& path, const AssetMetadata& meta) {
	JsonSerializer backend { meta.SourcePath->c_str() };

	backend.load_file();

	auto prefab { scope_cast<Prefab>(AssetFactory::get_instance().allocate_hollow_asset(AssetType::Prefab)) };
	prefab->backend = backend;

	return prefab;
}

}
