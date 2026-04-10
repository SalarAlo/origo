#pragma once

#include <algorithm>
#include <cmath>

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/ConeEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

namespace Origo {

struct SanitizeParticleEmissionShape {
	void operator()(PointEmissionShape& p) const {
		p.Position = sanitize_vec3(p.Position, Vec3 { 0.0f });
	}

	void operator()(SphereEmissionShape& sp) const {
		sp.Position = sanitize_vec3(sp.Position, Vec3 { 0.0f });
		sp.Radius = sanitize_positive(sp.Radius, 1.0f);
	}

	void operator()(BoxEmissionShape& b) const {
		b.Position = sanitize_vec3(b.Position, Vec3 { 0.0f });
		b.Size = Vec3 {
			sanitize_positive(b.Size.x, 1.0f),
			sanitize_positive(b.Size.y, 1.0f),
			sanitize_positive(b.Size.z, 1.0f)
		};
	}

	void operator()(ConeEmissionShape& c) const {
		c.Position = sanitize_vec3(c.Position, Vec3 { 0.0f });
		c.Direction = sanitize_direction(c.Direction);
		c.Angle = std::clamp(finite_or(c.Angle, 40.0f), 0.0f, 89.0f);
		c.Length = sanitize_positive(c.Length, 1.0f);
	}

private:
	static bool finite_vec3(const Vec3& v) {
		return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
	}

	static float finite_or(float value, float fallback) {
		return std::isfinite(value) ? value : fallback;
	}

	static float sanitize_positive(float value, float fallback) {
		value = finite_or(value, fallback);
		return std::max(0.001f, std::abs(value));
	}

	static Vec3 sanitize_vec3(const Vec3& value, const Vec3& fallback) {
		return finite_vec3(value) ? value : fallback;
	}

	static Vec3 sanitize_direction(const Vec3& direction) {
		if (!finite_vec3(direction) || length(direction) < 0.001f)
			return Vec3 { 0.0f, 1.0f, 0.0f };
		return direction;
	}
};

}
