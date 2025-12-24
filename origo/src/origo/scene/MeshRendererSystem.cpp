#include "origo/scene/MeshRenderSystem.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void MeshRenderSystem::Render(Scene* scene, RenderContext& context) {
	scene->View<MeshRenderer, Transform>(
	    [&](RID entity,
	        MeshRenderer& mr,
	        Transform& transform) {
		    context.Submit(
		        mr.GetMesh(),
		        mr.GetMaterial(),
		        &transform,
		        RenderPass::Geometry);
	    });
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderGeometry,
    Origo::MeshRenderSystem)
