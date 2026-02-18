#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

#include "origo/core/Random.h"

namespace Origo {
struct ParticleEmissionShapeSpawnPositionVisitor {
	Vec3 operator()(PointEmissionShape shape) {
		return shape.Position;
	}

	Vec3 operator()(SphereEmissionShape shape) {
		return shape.Position + (Random::RandomUnitVector() * shape.Radius * Random::Range(0.0f, 1.0f));
	}

	Vec3 operator()(BoxEmissionShape shape) {
		float halfSize = shape.Size / 2.0f;

		auto offset = Vec3 {
			Random::Range(-halfSize, halfSize),
			Random::Range(-halfSize, halfSize),
			Random::Range(-halfSize, halfSize)
		};

		return shape.Position + offset;
	}
};

}
