#include "EditorAssetVirtualPath.h"

namespace OrigoEditor {

std::string asset_type_to_string(Origo::AssetType type) {
	switch (type) {
	case Origo::AssetType::Texture2D:
		return "Textures";
	case Origo::AssetType::Material2D:
		return "Materials";
	case Origo::AssetType::Mesh:
		return "Meshes";
	case Origo::AssetType::Script:
		return "Scripts";
	case Origo::AssetType::Model:
		return "Models";
	case Origo::AssetType::Shader:
		return "Shaders";
	default:
		return "Other";
	}
}

std::filesystem::path compute_virtual_asset_path(const Origo::AssetMetadata& metadata) {
	if (!metadata.SourcePath.empty())
		return metadata.SourcePath.lexically_normal();

	std::filesystem::path root = "generated";

	switch (metadata.Origin) {
	case Origo::AssetOrigin::Runtime:
		root /= "runtime";
		break;
	case Origo::AssetOrigin::Synthetic:
		root /= "synthetic";
		break;
	default:
		root /= "unknown";
		break;
	}

	root /= asset_type_to_string(metadata.Type);
	root /= metadata.Name;

	return root;
}

}
