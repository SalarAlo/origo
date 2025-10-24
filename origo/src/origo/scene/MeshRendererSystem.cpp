#include "origo/scene/MeshRendererSystem.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/ComponentSystemRegistry.h"
#include "origo/scene/MeshRenderer.h"

namespace Origo {
void MeshRendererSystem::ForEach(const Scene& scene, const std::vector<Ref<MeshRenderer>>& instances) {
	for (const auto& instance : instances) {
		Renderer::Submit(
		    instance->GetMesh(),
		    instance->GetMaterial(),
		    scene.GetComponent<Transform>(instance->AttachedTo->GetId()));
	}
}
}

REGISTER_SYSTEM(MeshRendererSystem)
