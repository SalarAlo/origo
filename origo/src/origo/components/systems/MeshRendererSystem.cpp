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
		    if (!mr.MeshHandle.has_value() || !mr.MaterialHandle.has_value()) {
			    return;
		    }

		    context.SubmitMesh(
		        *mr.MeshHandle,
		        *mr.MaterialHandle,
		        transform.GetModelMatrix(),
		        RenderPass::Geometry);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::MeshRenderSystem)
