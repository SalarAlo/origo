#include "origo/assets/importers/ScriptImporter.h"
#include "origo/assets/Script.h"

namespace Origo {

bool ScriptImporter::CanImport(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	ToLowerInPlace(ext);
	return ext == ".lua";
}

AssetType ScriptImporter::GetAssetType() const {
	return AssetType::Script;
}

Scope<Asset> ScriptImporter::Import(const std::filesystem::path& path, const AssetMetadata& meta) {
	auto asset = MakeScope<Script>(path, meta.ID);

	return asset;
}

}
