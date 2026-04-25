#include "MeshRendererComponentSystem.h"

#include "origo/components/MeshRendererComponent.h"
#include "origo/components/TransformComponent.h"

#include "origo/scene/SystemScheduler.h"

namespace Origo {

void MeshRendererComponentSystem::render(Scene* scene, RenderContext& context) {
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
		        RenderPass::Geometry,
		        GL_TRIANGLES,
		        entity.get_id());
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::MeshRendererComponentSystem)
