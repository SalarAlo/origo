#include "origo/components/systems/ParticleSystemTransformInjecter.h"

#include "origo/components/Transform.h"

#include "origo/components/particle_system/ParticleSystemComponent.h"
#include "origo/components/particle_system/SetParticleEmissionShapeForward.h"
#include "origo/components/particle_system/SetParticleEmissionShapePosition.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void ParticleSystemTransformInjecter::update(Origo::Scene* scene, float dt) {
	scene->view<ParticleSystemComponent, TransformComponent>(
	    [&](RID emitterRID,
	        ParticleSystemComponent& particleSystem,
	        TransformComponent& particleSystemTransf) {
		    auto position_setter_visitor = SetParticleEmissionShapePosition { particleSystemTransf.get_position() };
		    std::visit(position_setter_visitor, particleSystem.Shape);

		    auto forward_setter_visitor = SetParticleEmissionShapeForward { particleSystemTransf.get_forward() };
		    std::visit(forward_setter_visitor, particleSystem.Shape);
	    });
}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateAlways,
    Origo::ParticleSystemTransformInjecter);
}
