#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ConeEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

#include "origo/core/Random.h"

namespace Origo {
struct SampleParticleEmissionPosition {
	Vec3 operator()(PointEmissionShape shape) {
		return shape.Position;
	}

	Vec3 operator()(ConeEmissionShape shape) {
		return shape.Position;
	}

	Vec3 operator()(SphereEmissionShape shape) {
		return shape.Position + (Random::random_unit_vector() * shape.Radius * Random::range(0.0f, 1.0f));
	}

	Vec3 operator()(BoxEmissionShape shape) {
		auto half_size = shape.Size / 2.0f;

		auto offset = Vec3 {
			Random::range(-half_size.x, half_size.x),
			Random::range(-half_size.y, half_size.y),
			Random::range(-half_size.z, half_size.z)
		};

		return shape.Position + offset;
	}
};

}
