#include "SceneAssetImporter.h"
#include "origo/assets/Asset.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/SceneAsset.h"
#include "origo/renderer/Helpers.h"

namespace Origo {
bool SceneAssetImporter::CanImport(const std::filesystem::path& path) const {
	auto stem { path.stem() };
	if (!stem.has_extension())
		return false;
	auto ext { stem.extension().string() };
	ToLowerInPlace(ext);
	return ext == ".scene";
}

AssetType SceneAssetImporter::GetAssetType() const {
	return AssetType::Scene;
}

Scope<Asset> SceneAssetImporter::Import(const std::filesystem::path& path, const AssetMetadata& meta) {
	auto scene { AssetFactory::AllocateHollowAsset<SceneAsset>() };
	scene->SetPath(path);
	return scene;
}

}
