#include "origo/components/systems/ParticleSystemTransformInjecter.h"

#include "origo/components/Transform.h"

#include "origo/components/particle_system/ParticleSystemComponent.h"
#include "origo/components/particle_system/SetParticleEmissionShapeForward.h"
#include "origo/components/particle_system/SetParticleEmissionShapePosition.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void ParticleSystemTransformInjecter::Update(Origo::Scene* scene, float dt) {
	scene->View<ParticleSystemComponent, TransformComponent>(
	    [&](RID emitterRID,
	        ParticleSystemComponent& particleSystem,
	        TransformComponent& particleSystemTransf) {
		    auto positionSetterVisitor = SetParticleEmissionShapePosition { particleSystemTransf.GetPosition() };
		    std::visit(positionSetterVisitor, particleSystem.Shape);

		    auto forwardSetterVisitor = SetParticleEmissionShapeForward { particleSystemTransf.GetForward() };
		    std::visit(forwardSetterVisitor, particleSystem.Shape);
	    });
}

REGISTER_UPDATE_SYSTEM(
    Origo::GamePhase::UpdateAlways,
    Origo::ParticleSystemTransformInjecter);
}
