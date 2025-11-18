#include "origo/scene/ModelRendererSystem.h"
#include "origo/renderer/Renderer.h"

namespace Origo {

void ModelRendererSystem::ForEach(const Scene& scene, const std::vector<ModelRenderer*>& instances) {
	for (const auto& renderer : instances) {
		for (const auto& submesh : renderer->GetModel()->GetSubMeshes()) {
			Renderer::Submit(
			    submesh.mesh,
			    submesh.material,
			    scene.GetComponent<Transform>(renderer->AttachedTo->GetId()));
		}
	}
}

}

// REGISTER_SYSTEM(ModelRendererSystem)
