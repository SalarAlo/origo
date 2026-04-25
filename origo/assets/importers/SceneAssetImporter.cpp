#include "SceneAssetImporter.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/SceneAsset.h"

#include "origo/renderer/Helpers.h"

namespace Origo {
bool SceneAssetImporter::can_import(const std::filesystem::path& path) const {
	auto stem { path.stem() };
	if (!stem.has_extension())
		return false;
	auto ext { stem.extension().string() };
	to_lower_in_place(ext);
	return ext == ".scene";
}

AssetType SceneAssetImporter::get_asset_type() const {
	return AssetType::Scene;
}

Scope<Asset> SceneAssetImporter::import(const std::filesystem::path& path, const AssetMetadata& meta) {
	auto scene { AssetFactory::get_instance().allocate_hollow_asset<SceneAsset>() };
	scene->set_path(path);
	return scene;
}

}
