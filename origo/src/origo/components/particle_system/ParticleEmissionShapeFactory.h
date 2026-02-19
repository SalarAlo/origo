
#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ParticleEmissionShape.h"
#include "origo/components/particle_system/ParticleEmissionShapeKind.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

namespace Origo {
struct ParticleEmissionShapeFactory {
	static ParticleEmissionShape CreateDefault(ParticleEmissionShapeKind kind) {
		switch (kind) {
		case ParticleEmissionShapeKind::Point:
			return PointEmissionShape {};
		case ParticleEmissionShapeKind::Sphere:
			return SphereEmissionShape { .Position = Vec3(0.0f), .Radius = 1.0f };
		case ParticleEmissionShapeKind::Cone:
			// TODO
			return SphereEmissionShape { .Position = Vec3(0.0f), .Radius = 1.0f };
		case ParticleEmissionShapeKind::Box:
			return BoxEmissionShape { .Position = Vec3(0.0f), .Size = 1.0f };
		}

		return PointEmissionShape {};
	}
};

}
