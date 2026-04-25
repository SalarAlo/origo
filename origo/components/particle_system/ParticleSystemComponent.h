#pragma once

#include <vector>

#include "ParticleEmissionShape.h"

#include "origo/assets/AssetManager.h"

#include "origo/components/Component.h"

namespace Origo {

struct ParticleBurst {
	float Time { 0.0f };
	int Count { 16 };
};

struct ParticleSystemComponent : public Component {
	std::string get_component_name() const override { return "ParticleSystem"; }

	bool IsLooping { true };
	float StartDelay { 0.0f };
	float SpawnRate { 10.0f };

	float StartSize { 0.4f };
	float EndSize { 0.0f };

	float LifetimeMin { 0.5f };
	float LifetimeMax { 1.0f };

	float InitialSpeedMin { 1.0f };
	float InitialSpeedMax { 3.0f };

	bool UseGravity { false };
	float GravityForceFactor { 1.0f };
	float Drag { 0.0f };

	Origo::OptionalAssetHandle ParticleMesh {};
	Origo::OptionalAssetHandle ParticleMaterial {};

	ParticleEmissionShape Shape {};
	std::vector<ParticleBurst> Bursts {};

	float SpawnAccumulator = 0.0f;
	float ElapsedTime = 0.0f;
};

}
