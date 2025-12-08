#include "origo/assets/importers/TextureImporter.h"
#include "origo/assets/Texture.h"
#include "origo/renderer/Helpers.h"

namespace Origo {

bool TextureImporter::CanImport(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	ToLowerInPlace(ext);
	return ext == ".jpg" || ext == ".jpeg" || ext == ".png";
}

AssetType TextureImporter::GetAssetType() const {
	return AssetType::Texture;
}

Scope<Asset> TextureImporter::Import(
    const std::filesystem::path& path,
    const AssetDescriptor& meta) {
	auto texture = MakeScope<Texture>(TextureType::Albedo);
	texture->SetSource(MakeScope<TextureSourceFile>(path.string()));
	return texture;
}

}
