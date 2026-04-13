#include "EditorAssetVirtualPath.h"

#include "origo/core/PathContext.h"

namespace OrigoEditor {

std::string asset_type_to_string(Origo::AssetType type) {
	switch (type) {
	case Origo::AssetType::Texture2D:
		return "Textures";
	case Origo::AssetType::Material:
	case Origo::AssetType::MaterialPBR:
	case Origo::AssetType::MaterialColor:
	case Origo::AssetType::TerrainMaterial:
	case Origo::AssetType::WaterMaterial:
	case Origo::AssetType::SkyboxMaterial:
		return "Materials";
	case Origo::AssetType::Mesh:
		return "Meshes";
	case Origo::AssetType::Script:
		return "Scripts";
	case Origo::AssetType::Model:
		return "Models";
	case Origo::AssetType::Shader:
		return "Shaders";
	case Origo::AssetType::Scene:
		return "Scenes";
	case Origo::AssetType::Audio:
		return "Audio";
	case Origo::AssetType::Prefab:
		return "Prefabs";
	default:
		return "Other";
	}
}

std::filesystem::path compute_virtual_asset_path(const Origo::AssetMetadata& metadata) {
	static const std::unordered_map<Origo::UUID, const Origo::AssetMetadata*> empty_index {};
	return compute_virtual_asset_path(metadata, empty_index);
}

std::filesystem::path compute_virtual_asset_path(
    const Origo::AssetMetadata& metadata,
    const std::unordered_map<Origo::UUID, const Origo::AssetMetadata*>& metadata_index) {
	if (metadata.SourcePath)
		return Origo::PathContextService::get_instance().serialize_path(*metadata.SourcePath);

	if (metadata.ParentID) {
		auto parent_it = metadata_index.find(*metadata.ParentID);
		if (parent_it != metadata_index.end() && parent_it->second) {
			const std::filesystem::path parent_path = compute_virtual_asset_path(*parent_it->second, metadata_index);
			return parent_path.parent_path() / metadata.Name;
		}
	}

	std::filesystem::path root = metadata.Origin == Origo::AssetOrigin::Synthetic
	    ? std::filesystem::path("_editor")
	    : std::filesystem::path("_runtime");

	root /= asset_type_to_string(metadata.Type);
	root /= metadata.Name;

	return root;
}

}
