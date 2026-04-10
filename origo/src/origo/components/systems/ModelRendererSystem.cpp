#include "origo/components/ModelRendererSystem.h"

#include "origo/components/ModelRenderer.h"
#include "origo/components/Transform.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void ModelRenderSystem::render(Scene* scene, RenderContext& context) {
	scene->view<ModelRendererComponent, TransformComponent>([&](RID entity, auto& mr, auto& tr) {
		if (!mr.ModelHandle.has_value())
			return;

		context.submit_model(*mr.ModelHandle, tr.get_model_matrix(), RenderPass::Geometry, std::nullopt, entity.get_id());
	});
}
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::ModelRenderSystem)
