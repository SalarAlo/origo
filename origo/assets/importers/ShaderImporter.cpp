#include "origo/assets/importers/ShaderImporter.h"

#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"

#include "origo/renderer/Helpers.h"

namespace Origo {

bool ShaderImporter::can_import(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	to_lower_in_place(ext);
	return ext == ".glsl";
}

AssetType ShaderImporter::get_asset_type() const {
	return AssetType::Shader;
}

Scope<Asset> ShaderImporter::import(const std::filesystem::path& path, const AssetMetadata& meta) {
	auto shader = make_scope<Shader>();
	shader->set_source(make_scope<ShaderSourceFile>(path.string()));
	return shader;
}

}
