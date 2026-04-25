#include "ParticleComponentSimulationSystem.h"

#include "origo/components/TransformComponent.h"

#include "origo/components/particle_system/ParticleComponent.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void ParticleComponentSimulationSystem::update(Scene* scene, float dt) {
	scene->view<ParticleComponent, TransformComponent>(
	    [&](RID entity, ParticleComponent& pc, TransformComponent& tr) {
		    if (pc.Lifetime <= 0.0f) {
			    scene->schedule_remove_entity(entity);
			    return;
		    }

		    if (pc.SimulatePhysics) {
			    if (pc.UseGravity)
				    pc.ForceAccum += Vec3(0.0f, -9.81f * pc.GravityForceFactor, 0.0f);

			    Vec3 acceleration = pc.ForceAccum;

			    pc.Velocity += acceleration * dt;

			    pc.Velocity *= 1.0f / (1.0f + pc.Drag * dt);

			    pc.ForceAccum = Vec3(0.0f);
		    }

		    tr.set_position(tr.get_position() + pc.Velocity * dt);
		    auto scale { glm::mix(Vec3 { pc.EndSize }, Vec3 { pc.StartSize }, pc.Lifetime / pc.MaxLifetime) };
		    tr.set_scale(scale);

		    pc.Lifetime -= dt;
	    });
}

}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateAlways,
    Origo::ParticleComponentSimulationSystem)
