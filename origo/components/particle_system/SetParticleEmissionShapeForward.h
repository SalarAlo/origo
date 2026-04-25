#pragma once

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ConeEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

namespace Origo {

struct SetParticleEmissionShapeForward {
	SetParticleEmissionShapeForward(const Vec3& forward)
	    : Forward(forward) { }

	void operator()(PointEmissionShape& p) {
	}

	void operator()(SphereEmissionShape& sp) {
	}

	void operator()(BoxEmissionShape& b) {
	}

	void operator()(ConeEmissionShape& b) {
		b.Direction = Forward;
	}

	const Vec3 Forward;
};

}
