
#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ConeEmissionShape.h"
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
			return ConeEmissionShape {
				.Position = Vec3(0.0f),
				.Direction = Vec3 { 0.0f, 1.0f, 0.0f },
				.Angle = 40.0f,
				.Length = 1.0f,
			};
		case ParticleEmissionShapeKind::Box:
			return BoxEmissionShape { .Position = Vec3(0.0f), .Size = Vec3 { 1.0f } };
		}

		return PointEmissionShape {};
	}
};

}
