#pragma once

#include "origo/serialization/ISerializer.h"

namespace Origo {

struct TerrainWaterSettings {
	bool enabled = true;
	float water_level = 0.18f;
	float wave_tile_scale = 0.085f;
	float wave_strength = 0.42f;
	float smoothness = 0.92f;

	void serialize(ISerializer& backend) const {
		backend.write("enabled", enabled);
		backend.write("water_level", water_level);
		backend.write("wave_tile_scale", wave_tile_scale);
		backend.write("wave_strength", wave_strength);
		backend.write("smoothness", smoothness);
	}

	void deserialize(ISerializer& backend) {
		backend.try_read("enabled", enabled);
		backend.try_read("water_level", water_level);
		backend.try_read("wave_tile_scale", wave_tile_scale);
		backend.try_read("wave_strength", wave_strength);
		backend.try_read("smoothness", smoothness);
	}
};

}
