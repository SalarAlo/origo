#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

#include "origo/core/Random.h"

namespace Origo {
struct SampleParticleEmissionDirection {
	Vec3 operator()(PointEmissionShape shape) {
		return Random::RandomUnitVector();
	}

	Vec3 operator()(SphereEmissionShape shape) {
		return Random::RandomUnitVector();
	}

	Vec3 operator()(BoxEmissionShape shape) {
		return Random::RandomUnitVector();
	}
};

}
