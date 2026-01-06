#include "origo/assets/importers/ModelImporter.h"
#include "origo/assets/Model.h"
#include "origo/renderer/Helpers.h"

namespace Origo {

bool ModelImporter::CanImport(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	ToLowerInPlace(ext);
	return ext == ".fbx" || ext == ".glb";
}

AssetType ModelImporter::GetAssetType() const {
	return AssetType::Model;
}

Scope<Asset> ModelImporter::Import(const std::filesystem::path& path, const AssetMetadata& meta) {
	auto asset = MakeScope<Model>();

	asset->SetPath(path);

	return asset;
}

}
