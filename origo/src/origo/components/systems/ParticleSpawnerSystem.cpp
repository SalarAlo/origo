#include "origo/components/systems/ParticleSpawnerSystem.h"

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

#include "origo/core/Random.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void ParticleSpawnerSystem::update(Origo::Scene* scene, float dt) {
	scene->view<ParticleSystemComponent, TransformComponent, NameComponent>(
	    [&](RID emitterRID,
	        ParticleSystemComponent& particleSystem,
	        TransformComponent& particleSystemTransf,
	        NameComponent& nc) {
		    particleSystem.SpawnAccumulator += particleSystem.SpawnRate * dt;

		    int spawn_count = (int)particleSystem.SpawnAccumulator;
		    particleSystem.SpawnAccumulator -= spawn_count;

		    for (size_t i = 0; i < spawn_count; i++) {
			    auto particle = scene->create_entity(
			        nc.Name + "_particle_" + std::to_string(i));

			    scene->add_native_component<EditorHiddenComponent>(particle);

			    auto* particle_transf = scene->get_native_component<TransformComponent>(particle);
			    particle_transf->set_position(std::visit(SampleParticleEmissionPosition {}, particleSystem.Shape));
			    particle_transf->set_scale(Vec3 { particleSystem.StartSize });

			    auto* pc = scene->add_native_component<ParticleComponent>(particle);

			    pc->StartSize = particleSystem.StartSize;
			    pc->EndSize = particleSystem.EndSize;

			    pc->OwnerEmitter = emitterRID;
			    pc->SimulatePhysics = true;

			    pc->UseGravity = particleSystem.UseGravity;
			    pc->GravityForceFactor = particleSystem.GravityForceFactor;
			    pc->Drag = particleSystem.Drag;

			    pc->Lifetime = Random::range(
			        particleSystem.LifetimeMin,
			        particleSystem.LifetimeMax);

			    pc->MaxLifetime = pc->Lifetime;

			    float speed = Random::range(
			        particleSystem.InitialSpeedMin,
			        particleSystem.InitialSpeedMax);

			    pc->Velocity = std::visit(SampleParticleEmissionDirection {}, particleSystem.Shape) * speed;

			    if (!particleSystem.ParticleMesh.has_value()) {
				    particleSystem.ParticleMesh = PrimitiveShapeCache::get_instance().get_sphere_mesh();
			    }
			    auto mesh = *particleSystem.ParticleMesh;

			    scene->add_native_component<MeshRendererComponent>(
			        particle,
			        DefaultAssetCache::get_instance().get_particle_material(),
			        mesh);
		    }
	    });
}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateGame,
    Origo::ParticleSpawnerSystem);

}
