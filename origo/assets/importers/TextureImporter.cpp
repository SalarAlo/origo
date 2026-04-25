#include "origo/assets/importers/TextureImporter.h"

#include "origo/assets/Texture2D.h"

#include "origo/renderer/Helpers.h"

namespace Origo {

bool TextureImporter::can_import(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	to_lower_in_place(ext);
	return ext == ".jpg" || ext == ".jpeg" || ext == ".png";
}

AssetType TextureImporter::get_asset_type() const {
	return AssetType::Texture2D;
}

Scope<Asset> TextureImporter::import(
    const std::filesystem::path& path,
    const AssetMetadata& meta) {
	auto texture = make_scope<Texture2D>(TextureType::Albedo);
	texture->set_source(make_scope<TextureSourceFile>(path.string()));
	return texture;
}

}
