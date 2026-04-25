#include "MaterialImporter.h"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetFactory.h"

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/renderer/Helpers.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {

bool MaterialImporter::can_import(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	to_lower_in_place(ext);
	return ext == ".matpbr";
}

AssetType MaterialImporter::get_asset_type() const {
	return AssetType::MaterialPBR;
}

Scope<Asset> MaterialImporter::import(const std::filesystem::path& path, const AssetMetadata& meta) {
	JsonSerializer backend { meta.SourcePath->c_str() };
	backend.load_file();
	auto serializer { AssetSerializationSystem::get(AssetType::MaterialPBR) };
	auto material { AssetFactory::get_instance().allocate_hollow_asset(AssetType::MaterialPBR) };
	serializer->deserialize(backend, *material.get());
	return material;
}

}
