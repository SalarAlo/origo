#include "origo/assets/importers/ModelImporter.h"

#include "origo/assets/AssetFactory.h"
#include "origo/assets/Model.h"

#include "origo/renderer/Helpers.h"

namespace Origo {

bool ModelImporter::can_import(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	to_lower_in_place(ext);
	return ext == ".fbx" || ext == ".glb";
}

AssetType ModelImporter::get_asset_type() const {
	return AssetType::Model;
}

Scope<Asset> ModelImporter::import(const std::filesystem::path& path, const AssetMetadata& meta) {
	auto asset = AssetFactory::get_instance().allocate_hollow_asset<Model>();

	asset->set_path(path);

	return asset;
}

}
