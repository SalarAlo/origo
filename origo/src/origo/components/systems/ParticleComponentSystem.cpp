#include "ParticleComponentSystem.h"

#include "origo/components/Transform.h"

#include "origo/components/particle_system/ParticleComponent.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void ParticleComponentSystem::Update(Scene* scene, float dt) {
	scene->View<ParticleComponent, TransformComponent>(
	    [&](RID entity, ParticleComponent& pc, TransformComponent& tr) {
		    if (pc.Lifetime <= 0.0f) {
			    scene->ScheduleRemoveEntity(entity);
			    return;
		    }

		    if (pc.SimulatePhysics) {
			    if (pc.UseGravity)
				    pc.ForceAccum += Vec3(0.0f, -9.81f * pc.Mass, 0.0f);

			    Vec3 acceleration = pc.ForceAccum / pc.Mass;

			    pc.Velocity += acceleration * dt;

			    pc.Velocity *= 1.0f / (1.0f + pc.Drag * dt);

			    pc.ForceAccum = Vec3(0.0f);
		    }

		    tr.SetPosition(tr.GetPosition() + pc.Velocity * dt);
		    auto scale { glm::mix(Vec3 { pc.EndSize }, Vec3 { pc.StartSize }, pc.Lifetime / pc.MaxLifetime) };
		    tr.SetScale(scale);

		    pc.Lifetime -= dt;
	    });
}

}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateAlways,
    Origo::ParticleComponentSystem)
