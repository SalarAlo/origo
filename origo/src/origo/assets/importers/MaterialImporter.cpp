#include "MaterialImporter.h"
#include "origo/assets/Asset.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/serialization/AssetSerializer.h"
#include "origo/renderer/Helpers.h"
#include "origo/serialization/JsonSerializer.h"

namespace Origo {

bool MaterialImporter::CanImport(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	ToLowerInPlace(ext);
	return ext == ".mat";
}

AssetType MaterialImporter::GetAssetType() const {
	return AssetType::Material2D;
}

Scope<Asset> MaterialImporter::Import(const std::filesystem::path& path, const AssetMetadata& meta) {
	JsonSerializer backend { meta.SourcePath.c_str() };
	backend.LoadFile();
	auto serializer { AssetSerializationSystem::Get(AssetType::Material2D) };
	auto material { AssetFactory::AllocateHollowAsset(AssetType::Material2D) };
	serializer->Deserialize(backend, *material.get());
	return material;
}

}
