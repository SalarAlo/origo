#include <variant>

#include "origo/components/systems/ParticleEmitterShapeDebugRenderSystem.h"

#include "origo/components/Transform.h"

#include "origo/components/particle_system/ParticleSystemComponent.h"
#include "origo/components/particle_system/RenderEmissionShapeDebug.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {
void ParticleEmitterShapeDebugRenderSystem::Render(Origo::Scene* scene, RenderContext& rCtx) {
	scene->View<ParticleSystemComponent, TransformComponent>(
	    [&](RID emitterRID,
	        ParticleSystemComponent& particleSystem,
	        TransformComponent& particleSystemTransf) {
		    std::visit(RenderEmissionShapeDebug { rCtx, particleSystemTransf }, particleSystem.Shape);
	    });
}
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    Origo::ParticleEmitterShapeDebugRenderSystem)
