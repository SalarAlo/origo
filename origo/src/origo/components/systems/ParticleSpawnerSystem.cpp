#include "origo/components/systems/ParticleSpawnerSystem.h"

#include <algorithm>
#include <vector>

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/EditorHiddenComponent.h"
#include "origo/components/MeshRenderer.h"
#include "origo/components/Name.h"
#include "origo/components/Transform.h"

#include "origo/components/particle_system/ParticleComponent.h"
#include "origo/components/particle_system/ParticleSystemComponent.h"
#include "origo/components/particle_system/SampleParticleEmissionDirection.h"
#include "origo/components/particle_system/SampleParticleEmissionPosition.h"
#include "origo/components/particle_system/SanitizeParticleEmissionShape.h"

#include "origo/core/Random.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

namespace {

struct PendingParticleSpawn {
	RID EmitterRID;
	size_t ParticleIndex;
};

float get_start_delay(const ParticleSystemComponent& particle_system) {
	return std::max(0.0f, particle_system.StartDelay);
}

float compute_continuous_emission_time(const ParticleSystemComponent& particle_system, float window_start, float window_end) {
	if (!particle_system.IsLooping || window_end <= window_start)
		return 0.0f;

	const float start_delay = get_start_delay(particle_system);
	return std::max(0.0f, window_end - std::max(window_start, start_delay));
}

int count_burst_triggers(const ParticleSystemComponent& particle_system, const ParticleBurst& burst, float window_start, float window_end) {
	if (window_end <= window_start || burst.Count <= 0)
		return 0;

	const float trigger_time = get_start_delay(particle_system) + std::max(0.0f, burst.Time);
	if (trigger_time == 0.0f && window_start == 0.0f && window_end > 0.0f)
		return 1;

	return window_start < trigger_time && trigger_time <= window_end ? 1 : 0;
}

void spawn_particle(
    Scene* scene,
    RID emitter_rid,
    ParticleSystemComponent& particle_system,
    const NameComponent& name,
    size_t particle_index) {
	auto particle = scene->create_entity(
	    name.Name + "_particle_" + std::to_string(particle_index));

	scene->add_native_component<EditorHiddenComponent>(particle);

	auto* particle_transform = scene->get_native_component<TransformComponent>(particle);
	particle_transform->set_position(std::visit(SampleParticleEmissionPosition {}, particle_system.Shape));
	particle_transform->set_scale(Vec3 { particle_system.StartSize });

	auto* particle_component = scene->add_native_component<ParticleComponent>(particle);

	particle_component->StartSize = particle_system.StartSize;
	particle_component->EndSize = particle_system.EndSize;

	particle_component->OwnerEmitter = emitter_rid;
	particle_component->SimulatePhysics = true;

	particle_component->UseGravity = particle_system.UseGravity;
	particle_component->GravityForceFactor = particle_system.GravityForceFactor;
	particle_component->Drag = particle_system.Drag;

	particle_component->Lifetime = Random::range(
	    particle_system.LifetimeMin,
	    particle_system.LifetimeMax);

	particle_component->MaxLifetime = particle_component->Lifetime;

	const float speed = Random::range(
	    particle_system.InitialSpeedMin,
	    particle_system.InitialSpeedMax);

	particle_component->Velocity = std::visit(SampleParticleEmissionDirection {}, particle_system.Shape) * speed;

	if (!particle_system.ParticleMesh.has_value())
		particle_system.ParticleMesh = PrimitiveShapeCache::get_instance().get_sphere_mesh();

	if (!particle_system.ParticleMaterial.has_value())
		particle_system.ParticleMaterial = DefaultAssetCache::get_instance().get_particle_material();

	scene->add_native_component<MeshRendererComponent>(
	    particle,
	    *particle_system.ParticleMaterial,
	    *particle_system.ParticleMesh);
}

}

void ParticleSpawnerSystem::update(Origo::Scene* scene, float dt) {
	constexpr int max_spawn_per_frame = 2048;
	std::vector<PendingParticleSpawn> pending_spawns {};

	scene->view<ParticleSystemComponent, TransformComponent, NameComponent>(
	    [&](RID emitterRID,
	        ParticleSystemComponent& particleSystem,
	        TransformComponent&,
	        NameComponent& nc) {
		    const float previous_time = particleSystem.ElapsedTime;
		    const float current_time = previous_time + std::max(0.0f, dt);
		    particleSystem.ElapsedTime = current_time;

		    const float continuous_emission_time = compute_continuous_emission_time(
		        particleSystem,
		        previous_time,
		        current_time);

		    particleSystem.SpawnAccumulator += std::max(0.0f, particleSystem.SpawnRate) * continuous_emission_time;

		    int spawn_count = static_cast<int>(particleSystem.SpawnAccumulator);
		    particleSystem.SpawnAccumulator -= spawn_count;

		    for (const auto& burst : particleSystem.Bursts)
			    spawn_count += count_burst_triggers(
			                       particleSystem,
			                       burst,
			                       previous_time,
			                       current_time)
			        * burst.Count;

		    spawn_count = std::min(spawn_count, max_spawn_per_frame);

		    for (size_t i = 0; i < static_cast<size_t>(spawn_count); ++i)
			    pending_spawns.push_back(PendingParticleSpawn {
			        .EmitterRID = emitterRID,
			        .ParticleIndex = i,
			    });
	    });

	for (const auto& pending_spawn : pending_spawns) {
		auto* particle_system = scene->get_native_component<ParticleSystemComponent>(pending_spawn.EmitterRID);
		auto* name = scene->get_native_component<NameComponent>(pending_spawn.EmitterRID);

		if (!particle_system || !name)
			continue;

		std::visit(SanitizeParticleEmissionShape {}, particle_system->Shape);

		spawn_particle(
		    scene,
		    pending_spawn.EmitterRID,
		    *particle_system,
		    *name,
		    pending_spawn.ParticleIndex);
	}
}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateGame,
    Origo::ParticleSpawnerSystem);

}
