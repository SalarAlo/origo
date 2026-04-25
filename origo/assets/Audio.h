#pragma once

#include "origo/assets/Asset.h"

namespace Origo {

class Audio : public Asset {
public:
	Audio(const std::optional<std::filesystem::path>& audio_path = std::nullopt);

	void play_sound();
	void resolve() override;

	AssetType get_asset_type() const { return AssetType::Audio; }
	static AssetType get_class_asset_type() { return AssetType::Audio; }

public:
	std::optional<std::filesystem::path> AudioPath { std::nullopt };
};

}
