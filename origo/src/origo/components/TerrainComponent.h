#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>

#include "origo/assets/AssetManager.h"
#include "origo/assets/MeshData.h"

#include "origo/components/Component.h"

#include "origo/components/terrain/ErosionSettings.h"

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
		active_build_task.reset();
		build_generation = 0;
	}

	Noise::Settings noise_settings {};
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
	std::shared_ptr<TerrainBuildTask> active_build_task {};
	uint64_t build_generation {};
};

}
