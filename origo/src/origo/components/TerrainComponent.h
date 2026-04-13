#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>

#include "origo/assets/AssetManager.h"
#include "origo/assets/MeshData.h"

#include "origo/components/Component.h"

#include "origo/components/terrain/ErosionSettings.h"
#include "origo/components/terrain/TerrainShapeSettings.h"
#include "origo/components/terrain/TerrainSurfaceSettings.h"
#include "origo/components/terrain/TerrainWaterSettings.h"

#include "origo/core/Random.h"
#include "origo/core/Typedefs.h"
namespace Origo {

struct TerrainBuildTask {
	std::atomic<bool> Ready { false };
	std::mutex ResultMutex {};
	std::optional<MeshData> Result { std::nullopt };
	uint64_t Generation { 0 };
};

struct TerrainComponent : public Component {
	TerrainComponent() = default;
	TerrainComponent(const TerrainComponent& other)
	    : noise_settings(other.noise_settings)
	    , shape_settings(other.shape_settings)
	    , surface_settings(other.surface_settings)
	    , water_settings(other.water_settings)
	    , use_texture_layers(other.use_texture_layers)
	    , ground_albedo(other.ground_albedo)
	    , ground_normal(other.ground_normal)
	    , ground_packed(other.ground_packed)
	    , rock_albedo(other.rock_albedo)
	    , rock_normal(other.rock_normal)
	    , rock_packed(other.rock_packed)
	    , sand_albedo(other.sand_albedo)
	    , sand_normal(other.sand_normal)
	    , sand_packed(other.sand_packed)
	    , snow_albedo(other.snow_albedo)
	    , snow_normal(other.snow_normal)
	    , snow_packed(other.snow_packed)
	    , micro_normal(other.micro_normal)
	    , erosion_settings(other.erosion_settings)
	    , size_x(other.size_x)
	    , size_z(other.size_z)
	    , scale(other.scale)
	    , cell_size(other.cell_size)
	    , height(other.height)
	    , contrast(other.contrast)
	    , should_rebuild(true)
	    , started_rebuilding(false) { }

	TerrainComponent& operator=(const TerrainComponent& other) {
		if (this == &other)
			return *this;

		noise_settings = other.noise_settings;
		shape_settings = other.shape_settings;
		surface_settings = other.surface_settings;
		water_settings = other.water_settings;
		use_texture_layers = other.use_texture_layers;
		ground_albedo = other.ground_albedo;
		ground_normal = other.ground_normal;
		ground_packed = other.ground_packed;
		rock_albedo = other.rock_albedo;
		rock_normal = other.rock_normal;
		rock_packed = other.rock_packed;
		sand_albedo = other.sand_albedo;
		sand_normal = other.sand_normal;
		sand_packed = other.sand_packed;
		snow_albedo = other.snow_albedo;
		snow_normal = other.snow_normal;
		snow_packed = other.snow_packed;
		micro_normal = other.micro_normal;
		erosion_settings = other.erosion_settings;
		size_x = other.size_x;
		size_z = other.size_z;
		scale = other.scale;
		cell_size = other.cell_size;
		height = other.height;
		contrast = other.contrast;
		reset_runtime_state();
		return *this;
	}

	std::string get_component_name() const override { return "TerrainComponent"; };

	void reset_runtime_state() {
		should_rebuild = true;
		started_rebuilding = false;
		terrain_mesh.reset();
		water_mesh.reset();
		terrain_material.reset();
		water_material.reset();
		active_build_task.reset();
		build_generation = 0;
	}

	Noise::Settings noise_settings {};
	TerrainShapeSettings shape_settings {};
	TerrainSurfaceSettings surface_settings {};
	TerrainWaterSettings water_settings {};
	bool use_texture_layers { false };
	OptionalAssetHandle ground_albedo { std::nullopt };
	OptionalAssetHandle ground_normal { std::nullopt };
	OptionalAssetHandle ground_packed { std::nullopt };
	OptionalAssetHandle rock_albedo { std::nullopt };
	OptionalAssetHandle rock_normal { std::nullopt };
	OptionalAssetHandle rock_packed { std::nullopt };
	OptionalAssetHandle sand_albedo { std::nullopt };
	OptionalAssetHandle sand_normal { std::nullopt };
	OptionalAssetHandle sand_packed { std::nullopt };
	OptionalAssetHandle snow_albedo { std::nullopt };
	OptionalAssetHandle snow_normal { std::nullopt };
	OptionalAssetHandle snow_packed { std::nullopt };
	OptionalAssetHandle micro_normal { std::nullopt };
	ErosionSettings erosion_settings {};

	int size_x { 500 };
	int size_z { 500 };
	float scale { 0.23f };
	float cell_size { 0.5f };
	float height { 50.0f };
	float contrast { 1.0f };

	// runtime stuff
	bool should_rebuild { true };
	bool started_rebuilding {};

	OptionalAssetHandle terrain_mesh { std::nullopt };
	OptionalAssetHandle water_mesh { std::nullopt };
	OptionalAssetHandle terrain_material { std::nullopt };
	OptionalAssetHandle water_material { std::nullopt };
	std::shared_ptr<TerrainBuildTask> active_build_task {};
	uint64_t build_generation {};
};

}
