#pragma once

#include <glm/glm.hpp>

#include "origo/components/Component.h"

#include "origo/core/RID.h"

namespace Origo {

struct ParticleComponent : public Component {
	RID OwnerEmitter;

	Vec3 Velocity { 0.0f };

	float StartSize {};
	float EndSize {};

	float GravityForceFactor = 1.0f;
	float Drag = 0.0f;

	bool UseGravity = true;
	bool SimulatePhysics = true;

	Vec3 ForceAccum { 0.0f };

	float Lifetime = 0.0f;
	float MaxLifetime = 0.0f;

	std::string GetComponentName() const override { return "Particle"; }
};

}
