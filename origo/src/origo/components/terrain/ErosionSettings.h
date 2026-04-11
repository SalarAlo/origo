#pragma once

#include "origo/serialization/ISerializer.h"

namespace Origo {

struct ErosionSettings {
	int seed = 1337;

	int droplet_count = 50000;
	int max_lifetime = 30;

	float sediment_capacity_factor = 4.0f;
	float min_sediment_capacity = 0.01f;

	float erode_speed = 0.3f;
	float deposit_speed = 0.3f;

	float evaporate_speed = 0.01f;

	float gravity = 4.0f;
	float inertia = 0.05f;

	void serialize(ISerializer& backend) {
		backend.write("seed", seed);
		backend.write("droplet_count", droplet_count);
		backend.write("max_lifetime", max_lifetime);
		backend.write("sediment_capacity_factor", sediment_capacity_factor);
		backend.write("min_sediment_capacity", min_sediment_capacity);
		backend.write("erode_speed", erode_speed);
		backend.write("deposit_speed", deposit_speed);
		backend.write("evaporate_speed", evaporate_speed);
		backend.write("gravity", gravity);
		backend.write("inertia", inertia);
	}

	void deserialize(ISerializer& backend) {
		backend.try_read("seed", seed);
		backend.try_read("droplet_count", droplet_count);
		backend.try_read("max_lifetime", max_lifetime);
		backend.try_read("sediment_capacity_factor", sediment_capacity_factor);
		backend.try_read("min_sediment_capacity", min_sediment_capacity);
		backend.try_read("erode_speed", erode_speed);
		backend.try_read("deposit_speed", deposit_speed);
		backend.try_read("evaporate_speed", evaporate_speed);
		backend.try_read("gravity", gravity);
		backend.try_read("inertia", inertia);
	}
};

}
