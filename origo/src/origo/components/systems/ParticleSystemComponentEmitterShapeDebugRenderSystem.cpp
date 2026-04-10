#include <variant>

#include "origo/components/systems/ParticleSystemComponentEmitterShapeDebugRenderSystem.h"

#include "origo/components/TransformComponent.h"

#include "origo/components/particle_system/ParticleSystemComponent.h"
#include "origo/components/particle_system/RenderEmissionShapeDebug.h"
#include "origo/components/particle_system/SanitizeParticleEmissionShape.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {
void ParticleSystemComponentEmitterShapeDebugRenderSystem::render(Origo::Scene* scene, RenderContext& rCtx) {
	scene->view<ParticleSystemComponent, TransformComponent>(
	    [&](RID emitterRID,
	        ParticleSystemComponent& particleSystem,
	        TransformComponent& particleSystemTransf) {
		    std::visit(SanitizeParticleEmissionShape {}, particleSystem.Shape);
		    std::visit(RenderEmissionShapeDebug { rCtx, particleSystemTransf }, particleSystem.Shape);
	    });
}
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    Origo::ParticleSystemComponentEmitterShapeDebugRenderSystem)
