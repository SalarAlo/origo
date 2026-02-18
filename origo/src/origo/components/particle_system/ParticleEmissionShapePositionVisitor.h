#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

namespace Origo {
struct ParticleEmissionShapePositionVisitor {
	ParticleEmissionShapePositionVisitor(const Vec3& position)
	    : Position(position) { }

	void operator()(PointEmissionShape& p) {
		p.Position = Position;
	}

	void operator()(SphereEmissionShape& sp) {
		sp.Position = Position;
	}

	void operator()(BoxEmissionShape& b) {
		b.Position = Position;
	}

	Vec3 Position;
};

}
