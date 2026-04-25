#include "origo/assets/importers/MaterialColorImporter.h"

#include "origo/assets/AssetFactory.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/renderer/Helpers.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {

bool MaterialColorImporter::can_import(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	to_lower_in_place(ext);
	return ext == ".matcolor";
}

AssetType MaterialColorImporter::get_asset_type() const {
	return AssetType::MaterialColor;
}

Scope<Asset> MaterialColorImporter::import(const std::filesystem::path& path, const AssetMetadata& meta) {
	JsonSerializer backend { meta.SourcePath->c_str() };
	backend.load_file();
	auto serializer { AssetSerializationSystem::get(AssetType::MaterialColor) };
	auto material { AssetFactory::get_instance().allocate_hollow_asset(AssetType::MaterialColor) };
	serializer->deserialize(backend, *material.get());
	return material;
}

}
