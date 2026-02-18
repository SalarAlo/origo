#pragma once

#include "glm/ext/vector_float3.hpp"

#include "origo/components/Component.h"

namespace Origo {

struct ParticleSystemComponent : public Component {
	std::string GetComponentName() const override { return "ParticleSystem"; }

	bool IsLooping { true };
	float SpawnSpeed { 1.0f };
	glm::vec3 StartSize { glm::vec3(1.0f) };
};

}
