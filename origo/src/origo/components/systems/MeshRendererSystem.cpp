#include "MeshRenderSystem.h"

#include "origo/components/MeshRenderer.h"
#include "origo/components/Transform.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void MeshRenderSystem::render(Scene* scene, RenderContext& context) {
	scene->view<MeshRendererComponent, TransformComponent>(
	    [&](RID entity,
	        MeshRendererComponent& mr,
	        TransformComponent& transform) {
		    if (!mr.MeshHandle.has_value() || !mr.MaterialHandle.has_value()) {
			    return;
		    }

		    context.submit_mesh(
		        *mr.MeshHandle,
		        *mr.MaterialHandle,
		        transform.get_model_matrix(),
		        RenderPass::Geometry);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::MeshRenderSystem)
