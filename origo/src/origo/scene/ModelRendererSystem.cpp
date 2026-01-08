#include "origo/scene/ModelRendererSystem.h"
#include "origo/assets/Model.h"
#include "origo/scene/ModelRenderer.h"
#include "origo/scene/SystemScheduler.h"
#include "origo/scene/Transform.h"

namespace Origo {

void ModelRenderSystem::Render(Scene* scene, RenderContext& context) {
	scene->View<ModelRenderer, Transform>(
	    [&](RID entity,
	        ModelRenderer& mr,
	        Transform& transform) {
		    auto modelHandle = mr.GetModel();
		    if (!modelHandle.has_value())
			    return;

		    auto model = AssetManager::GetInstance().Get<Model>(*modelHandle);
		    if (!model)
			    return;

		    const auto& nodes = model->GetNodes();
		    const auto& subMeshes = model->GetSubMeshes();

		    std::vector<glm::mat4> worldMatrices(nodes.size());

		    worldMatrices[model->GetRootNode()] = transform.GetModelMatrix() * nodes[model->GetRootNode()].LocalTransform;

		    for (size_t i = 0; i < nodes.size(); ++i) {
			    const auto& node = nodes[i];

			    if (node.Parent != -1) {
				    worldMatrices[i] = worldMatrices[node.Parent] * node.LocalTransform;
			    }

			    if (node.SubMeshIndex != -1) {
				    const auto& sub = subMeshes[node.SubMeshIndex];

				    context.Submit(
				        sub.Mesh,
				        sub.Material,
				        worldMatrices[i],
				        RenderPass::Geometry);
			    }
		    }
	    });
}
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderGeometry,
    Origo::ModelRenderSystem)
