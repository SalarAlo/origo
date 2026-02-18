#pragma once

#include "ParticleEmissionShape.h"

#include "origo/assets/AssetManager.h"

#include "origo/components/Component.h"

namespace Origo {

struct ParticleSystemComponent : public Component {
	std::string GetComponentName() const override { return "ParticleSystem"; }

	bool IsLooping { true };
	float SpawnRate { 10.0f };

	float StartSize { 1.0f };
	float EndSize { 1.0f };

	float LifetimeMin { 0.5f };
	float LifetimeMax { 2.0f };

	float InitialSpeedMin { 1.0f };
	float InitialSpeedMax { 5.0f };

	bool UseGravity { false };
	float Mass { 1.0f };
	float Drag { 0.0f };

	Origo::OptionalAssetHandle ParticleMesh;

	ParticleEmissionShape Shape {};
};

}
