#include "origo/assets/importers/AudioImporter.h"

#include "origo/assets/Audio.h"

#include "origo/renderer/Helpers.h"

namespace Origo {

bool AudioImporter::can_import(const std::filesystem::path& path) const {
	auto ext = path.extension().string();
	to_lower_in_place(ext);
	return ext == ".mp3" || ext == ".wav";
}

AssetType AudioImporter::get_asset_type() const {
	return AssetType::Audio;
}

Scope<Asset> AudioImporter::import(const std::filesystem::path& path, const AssetMetadata& meta) {
	auto asset = make_scope<Audio>(path);

	return asset;
}

}
