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
		return Random::random_unit_vector();
	}

	Vec3 operator()(SphereEmissionShape shape) {
		return Random::random_unit_vector();
	}

	Vec3 operator()(BoxEmissionShape shape) {
		return Random::random_unit_vector();
	}

	Vec3 operator()(ConeEmissionShape shape) {
		Vec3 axis = normalize(shape.Direction);

		float angle_rad = glm::radians(shape.Angle);
		float radius = shape.Length * tanf(angle_rad);
		float cos_theta = cosf(angle_rad);

		float u = Random::float01();
		float v = Random::float01();

		float cos_phi = 1.0f - u * (1.0f - cos_theta);
		float sin_phi = sqrtf(1.0f - cos_phi * cos_phi);
		float azimuth = 2.0f * glm::pi<float>() * v;

		Vec3 local_dir {
			cosf(azimuth) * sin_phi,
			cos_phi,
			sinf(azimuth) * sin_phi
		};

		Vec3 up = fabs(axis.y) < 0.999f ? Vec3 { 0, 1, 0 } : Vec3 { 1, 0, 0 };
		Vec3 right = normalize(cross(up, axis));
		Vec3 forward = cross(axis, right);

		return normalize(
		           right * local_dir.x + axis * local_dir.y + forward * local_dir.z)
		    * shape.Length;
	}
};

}
