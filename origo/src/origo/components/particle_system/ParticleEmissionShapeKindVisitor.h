#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ParticleEmissionShapeKind.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

namespace Origo {
struct ParticleEmissionShapeKindVisitor {
	ParticleEmissionShapeKind operator()(PointEmissionShape _) {
		return ParticleEmissionShapeKind::Point;
	}

	ParticleEmissionShapeKind operator()(SphereEmissionShape _) {
		return ParticleEmissionShapeKind::Sphere;
	}

	ParticleEmissionShapeKind operator()(BoxEmissionShape _) {
		return ParticleEmissionShapeKind::Box;
	}
};

}
