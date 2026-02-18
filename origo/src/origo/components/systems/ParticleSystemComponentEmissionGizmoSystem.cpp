#include <variant>

#include "origo/components/systems/ParticleSystemComponentEmissionGizmoSystem.h"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/Transform.h"

#include "origo/components/particle_system/ParticleEmissionShapeKindVisitor.h"
#include "origo/components/particle_system/ParticleSystemComponent.h"

#include "origo/renderer/RenderCommand.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {
void ParticleSystemComponentEmissionGizmoSystem::Render(Origo::Scene* scene, RenderContext& rCtx) {
	scene->View<ParticleSystemComponent, TransformComponent>(
	    [&](RID emitterRID,
	        ParticleSystemComponent& particleSystem,
	        TransformComponent& particleSystemTransf) {
		    switch (std::visit(ParticleEmissionShapeKindVisitor {}, particleSystem.Shape)) {
		    case ParticleEmissionShapeKind::Point:
			    break;
		    case ParticleEmissionShapeKind::Sphere: {
			    TransformComponent transform;
			    transform.SetPosition(particleSystemTransf.GetPosition());

			    rCtx.SubmitMesh(
			        PrimitiveShapeCache::GetInstance().GetSphereMesh(),
			        DefaultAssetCache::GetInstance().GetMaterial(),
			        transform.GetModelMatrix(),
			        RenderPass::Geometry,
			        GL_LINES);
			    break;
		    }
		    case ParticleEmissionShapeKind::Cone: {
			    TransformComponent transform;
			    transform.SetPosition(particleSystemTransf.GetPosition());

			    rCtx.SubmitMesh(
			        PrimitiveShapeCache::GetInstance().GetConeMesh(),
			        DefaultAssetCache::GetInstance().GetMaterial(),
			        transform.GetModelMatrix(),
			        RenderPass::Geometry,
			        GL_LINES);
			    break;
		    }
		    case ParticleEmissionShapeKind::Box: {
			    TransformComponent transform;
			    transform.SetPosition(particleSystemTransf.GetPosition());

			    rCtx.SubmitMesh(
			        PrimitiveShapeCache::GetInstance().GetCubeMesh(),
			        DefaultAssetCache::GetInstance().GetMaterial(),
			        transform.GetModelMatrix(),
			        RenderPass::Geometry,
			        GL_LINE_LOOP);
			    break;
		    }
		    }
	    });
}
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderEditor,
    Origo::ParticleSystemComponentEmissionGizmoSystem)
