#include "MeshRenderSystem.h"

#include "origo/components/MeshRenderer.h"
#include "origo/components/Transform.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void MeshRenderSystem::Render(Scene* scene, RenderContext& context) {
	scene->View<MeshRendererComponent, TransformComponent>(
	    [&](RID entity,
	        MeshRendererComponent& mr,
	        TransformComponent& transform) {
		    if (!mr.GetMesh().has_value() || !mr.GetMaterial().has_value()) {
			    return;
		    }

		    context.SubmitMesh(
		        *mr.GetMesh(),
		        *mr.GetMaterial(),
		        transform.GetModelMatrix(),
		        RenderPass::Geometry);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::MeshRenderSystem)
