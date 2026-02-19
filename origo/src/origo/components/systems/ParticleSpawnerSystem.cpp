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
#include "origo/components/particle_system/SetParticleEmissionShapePosition.h"

#include "origo/core/Random.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void ParticleSpawnerSystem::Render(Scene* scene, RenderContext& rCtx) {
	scene->View<ParticleSystemComponent, TransformComponent, NameComponent>(
	    [&](RID emitterRID,
	        ParticleSystemComponent& particleSystem,
	        TransformComponent& particleSystemTransf,
	        NameComponent& nc) {
		    auto positionSetterVisitor = SetParticleEmissionShapePosition { particleSystemTransf.GetPosition() };
		    std::visit(positionSetterVisitor, particleSystem.Shape);

		    for (size_t i = 0; i < particleSystem.SpawnRate; i++) {
			    auto particle = scene->CreateEntity(
			        nc.Name + "_particle_" + std::to_string(i));

			    scene->AddNativeComponent<EditorHiddenComponent>(particle);

			    auto* particleTransf = scene->GetNativeComponent<TransformComponent>(particle);
			    particleTransf->SetPosition(std::visit(SampleParticleEmissionPosition {}, particleSystem.Shape));
			    particleTransf->SetScale(Vec3 { particleSystem.StartSize });

			    auto* pc = scene->AddNativeComponent<ParticleComponent>(particle);

			    pc->StartSize = particleSystem.StartSize;
			    pc->EndSize = particleSystem.EndSize;

			    pc->OwnerEmitter = emitterRID;
			    pc->SimulatePhysics = true;

			    pc->UseGravity = particleSystem.UseGravity;
			    pc->GravityForceFactor = particleSystem.GravityForceFactor;
			    pc->Drag = particleSystem.Drag;

			    pc->Lifetime = Random::Range(
			        particleSystem.LifetimeMin,
			        particleSystem.LifetimeMax);

			    pc->MaxLifetime = pc->Lifetime;

			    float speed = Random::Range(
			        particleSystem.InitialSpeedMin,
			        particleSystem.InitialSpeedMax);

			    pc->Velocity = std::visit(SampleParticleEmissionDirection {}, particleSystem.Shape) * speed;

			    auto mesh = particleSystem.ParticleMesh.has_value()
			        ? *particleSystem.ParticleMesh
			        : PrimitiveShapeCache::GetInstance().GetSphereMesh();

			    scene->AddNativeComponent<MeshRendererComponent>(
			        particle,
			        DefaultAssetCache::GetInstance().GetMaterial(),
			        mesh);
		    }
	    });
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderGame,
    Origo::ParticleSpawnerSystem);

}
