#include "origo/scene/MeshRenderSystem.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void MeshRenderSystem::Render(const Scene& scene, RenderContext& context) {
	auto meshRenderers = scene.GetAllComponentsOfType<MeshRenderer>();

	for (auto* mr : meshRenderers) {
		auto* transform = scene.GetComponent<Transform>(mr->AttachedTo->GetId());
		if (!transform)
			continue;

		context.Submit(
		    mr->GetMesh(),
		    mr->GetMaterial(),
		    transform,
		    RenderPass::Geometry);
	}
}

}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderGeometry,
    Origo::MeshRenderSystem)
