#pragma once

#include "origo/serialization/ISerializer.h"

namespace Origo {

struct TerrainShapeSettings {
	float base_frequency_multiplier = 0.28f;
	float ridge_frequency_multiplier = 1.55f;
	float medium_frequency_multiplier = 0.85f;
	float fine_frequency_multiplier = 3.4f;

	float base_strength = 0.64f;
	float ridge_strength = 0.30f;
	float medium_detail_strength = 0.18f;
	float detail_strength = 0.07f;

	float domain_warp_strength = 18.0f;
	float remap_min = 0.10f;
	float remap_max = 0.95f;

	void serialize(ISerializer& backend) const {
		backend.write("base_frequency_multiplier", base_frequency_multiplier);
		backend.write("ridge_frequency_multiplier", ridge_frequency_multiplier);
		backend.write("medium_frequency_multiplier", medium_frequency_multiplier);
		backend.write("fine_frequency_multiplier", fine_frequency_multiplier);
		backend.write("base_strength", base_strength);
		backend.write("ridge_strength", ridge_strength);
		backend.write("medium_detail_strength", medium_detail_strength);
		backend.write("detail_strength", detail_strength);
		backend.write("domain_warp_strength", domain_warp_strength);
		backend.write("remap_min", remap_min);
		backend.write("remap_max", remap_max);
	}

	void deserialize(ISerializer& backend) {
		backend.try_read("base_frequency_multiplier", base_frequency_multiplier);
		backend.try_read("ridge_frequency_multiplier", ridge_frequency_multiplier);
		backend.try_read("medium_frequency_multiplier", medium_frequency_multiplier);
		backend.try_read("fine_frequency_multiplier", fine_frequency_multiplier);
		backend.try_read("base_strength", base_strength);
		backend.try_read("ridge_strength", ridge_strength);
		backend.try_read("medium_detail_strength", medium_detail_strength);
		backend.try_read("detail_strength", detail_strength);
		backend.try_read("domain_warp_strength", domain_warp_strength);
		backend.try_read("remap_min", remap_min);
		backend.try_read("remap_max", remap_max);
	}
};

}
