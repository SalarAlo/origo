#include "origo/assets/importers/ScriptImporter.h"

#include "origo/assets/Script.h"

namespace Origo {

bool ScriptImporter::can_import(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	to_lower_in_place(ext);
	return ext == ".lua";
}

AssetType ScriptImporter::get_asset_type() const {
	return AssetType::Script;
}

Scope<Asset> ScriptImporter::import(const std::filesystem::path& path, const AssetMetadata& meta) {
	auto asset = make_scope<Script>(path, *meta.ID);

	return asset;
}

}
