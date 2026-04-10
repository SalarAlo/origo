#pragma once

#include "origo/assets/Mesh.h"

#include "origo/components/Component.h"

#include "origo/core/Random.h"
#include "origo/core/Typedefs.h"

namespace Origo {

struct TerrainComponent : public Component {
	TerrainComponent() = default;
	TerrainComponent(const TerrainComponent& other)
	    : settings(other.settings)
	    , scale(other.scale)
	    , contrast(other.contrast)
	    , should_rebuild(true)
	    , started_rebuilding(false) { }

	TerrainComponent& operator=(const TerrainComponent& other) {
		if (this == &other)
			return *this;

		settings = other.settings;
		scale = other.scale;
		contrast = other.contrast;
		reset_runtime_state();
		return *this;
	}

	std::string get_component_name() const override { return "TerrainComponent"; };

	void reset_runtime_state() {
		should_rebuild = true;
		started_rebuilding = false;
		terrain_mesh.reset();
	}

	Noise::Settings settings {};
	float scale {};
	float contrast {};

	// runtime stuff
	bool should_rebuild {};
	bool started_rebuilding {};

	Scope<Mesh> terrain_mesh {};
};

}
