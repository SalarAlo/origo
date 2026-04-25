#pragma once

#include "origo/serialization/ISerializer.h"

namespace Origo {

struct TerrainSurfaceSettings {
	float ground_tile_scale = 0.085f;
	float rock_tile_scale = 0.060f;
	float sand_tile_scale = 0.110f;
	float snow_tile_scale = 0.070f;
	float micro_detail_scale = 0.240f;
	float micro_detail_strength = 0.28f;

	float rock_slope_start = 0.18f;
	float rock_slope_end = 0.46f;
	float cliff_slope_start = 0.32f;
	float cliff_slope_end = 0.68f;

	float shoreline_width = 0.055f;
	float wetness_width = 0.040f;
	float snow_start = 0.78f;
	float snow_end = 0.93f;
	float snow_slope_limit = 0.24f;
	float height_blend_sharpness = 0.35f;

	void serialize(ISerializer& backend) const {
		backend.write("ground_tile_scale", ground_tile_scale);
		backend.write("rock_tile_scale", rock_tile_scale);
		backend.write("sand_tile_scale", sand_tile_scale);
		backend.write("snow_tile_scale", snow_tile_scale);
		backend.write("micro_detail_scale", micro_detail_scale);
		backend.write("micro_detail_strength", micro_detail_strength);
		backend.write("rock_slope_start", rock_slope_start);
		backend.write("rock_slope_end", rock_slope_end);
		backend.write("cliff_slope_start", cliff_slope_start);
		backend.write("cliff_slope_end", cliff_slope_end);
		backend.write("shoreline_width", shoreline_width);
		backend.write("wetness_width", wetness_width);
		backend.write("snow_start", snow_start);
		backend.write("snow_end", snow_end);
		backend.write("snow_slope_limit", snow_slope_limit);
		backend.write("height_blend_sharpness", height_blend_sharpness);
	}

	void deserialize(ISerializer& backend) {
		backend.try_read("ground_tile_scale", ground_tile_scale);
		backend.try_read("rock_tile_scale", rock_tile_scale);
		backend.try_read("sand_tile_scale", sand_tile_scale);
		backend.try_read("snow_tile_scale", snow_tile_scale);
		backend.try_read("micro_detail_scale", micro_detail_scale);
		backend.try_read("micro_detail_strength", micro_detail_strength);
		backend.try_read("rock_slope_start", rock_slope_start);
		backend.try_read("rock_slope_end", rock_slope_end);
		backend.try_read("cliff_slope_start", cliff_slope_start);
		backend.try_read("cliff_slope_end", cliff_slope_end);
		backend.try_read("shoreline_width", shoreline_width);
		backend.try_read("wetness_width", wetness_width);
		backend.try_read("snow_start", snow_start);
		backend.try_read("snow_end", snow_end);
		backend.try_read("snow_slope_limit", snow_slope_limit);
		backend.try_read("height_blend_sharpness", height_blend_sharpness);
	}
};

}
