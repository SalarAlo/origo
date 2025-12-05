#include "origo/assets/importers/TextureImporter.h"
#include "origo/assets/AssetMetadata.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/Texture.h"
#include "origo/renderer/Helpers.h"
#include "origo/serialization/JsonSerializer.h"
#include "origo/core/Logger.h"

namespace Origo {

bool TextureImporter::CanImport(const std::filesystem::path& path) {
	auto ext = path.extension().string();
	ToLowerInPlace(ext);
	return ext == ".jpg" || ext == ".jpeg" || ext == ".png";
}

AssetType TextureImporter::GetAssetType() {
	return AssetType::Texture;
}

void TextureImporter::Import(const std::filesystem::path& path, const AssetMetadata& meta) {
	std::filesystem::path assetPath = path.string() + ".asset";

	JsonSerializer backend(assetPath.string());
	Texture tempTexture(TextureType::Albedo);
	tempTexture.SetSource(MakeScope<TextureSourceFile>(path.string()));

	auto serializer = AssetSerializationSystem::Get(GetAssetType());
	if (!serializer) {
		ORG_ERROR("TextureImporter: No serializer registered for Texture assets.");
		return;
	}

	serializer->Serialize(&tempTexture, backend);
	backend.SaveToFile();
}

}
