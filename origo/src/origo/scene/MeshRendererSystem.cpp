#include "origo/renderer/RenderContext.h"
#include "origo/renderer/RenderableRegistry.h"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Scene.h"

namespace Origo {
static void RenderMeshRenderers(const Scene& scene, RenderContext& context) {
	auto meshRenderers = scene.GetAllComponentsOfType<MeshRenderer>();

	for (auto* mr : meshRenderers) {
		auto* transform = scene.GetComponent<Transform>(mr->AttachedTo->GetId());

		context.Submit(
		    mr->GetMesh(),
		    mr->GetMaterial(),
		    transform);
	}
}

REGISTER_RENDERABLE(RenderMeshRenderers)

}
