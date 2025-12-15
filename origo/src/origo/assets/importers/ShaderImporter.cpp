#include "origo/assets/importers/ShaderImporter.h"
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"
#include "origo/renderer/Helpers.h"

namespace Origo {

bool ShaderImporter::CanImport(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	ToLowerInPlace(ext);
	return ext == ".glsl";
}

AssetType ShaderImporter::GetAssetType() const {
	return AssetType::Shader;
}

Scope<Asset> ShaderImporter::Import(const std::filesystem::path& path, const Metadata& meta) {
	auto shader = MakeScope<Shader>();
	shader->SetSource(MakeScope<ShaderSourceFile>(path.string()));
	return shader;
}

}
