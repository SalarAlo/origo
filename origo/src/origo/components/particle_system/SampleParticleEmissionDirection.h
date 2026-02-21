#pragma once

#include "glm/ext/scalar_constants.hpp"

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ConeEmissionShape.h"
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

	Vec3 operator()(ConeEmissionShape shape) {
		Vec3 axis = normalize(shape.Direction);

		float angleRad = glm::radians(shape.Angle);
		float radius = shape.Length * tanf(angleRad);
		float cosTheta = cosf(angleRad);

		float u = Random::Float01();
		float v = Random::Float01();

		float cosPhi = 1.0f - u * (1.0f - cosTheta);
		float sinPhi = sqrtf(1.0f - cosPhi * cosPhi);
		float azimuth = 2.0f * glm::pi<float>() * v;

		Vec3 localDir {
			cosf(azimuth) * sinPhi,
			cosPhi,
			sinf(azimuth) * sinPhi
		};

		Vec3 up = fabs(axis.y) < 0.999f ? Vec3 { 0, 1, 0 } : Vec3 { 1, 0, 0 };
		Vec3 right = normalize(cross(up, axis));
		Vec3 forward = cross(axis, right);

		return normalize(
		    right * localDir.x + axis * localDir.y + forward * localDir.z);
	}
};

}
