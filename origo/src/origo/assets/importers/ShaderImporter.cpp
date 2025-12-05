#include "origo/assets/importers/ShaderImporter.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"
#include "origo/renderer/Helpers.h"
#include "origo/serialization/JsonSerializer.h"

namespace Origo {

bool ShaderImporter::CanImport(const std::filesystem::path& path) {
	auto ext = path.extension().string();
	ToLowerInPlace(ext);
	return ext == ".glsl";
}

AssetType ShaderImporter::GetAssetType() {
	return AssetType::Shader;
}

void ShaderImporter::Import(const std::filesystem::path& path, const AssetMetadata& meta) {
	std::filesystem::path assetPath = path.string() + ".asset";

	JsonSerializer backend(assetPath.string());
	Shader tempShader {};
	tempShader.SetSource(MakeScope<ShaderSourceFile>(path.string()));

	auto serializer = AssetSerializationSystem::Get(GetAssetType());
	if (!serializer) {
		ORG_ERROR("ShaderImporter: No serializer registered for Texture assets.");
		return;
	}

	serializer->Serialize(&tempShader, backend);
	backend.SaveToFile();
}

}
