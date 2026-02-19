#include "origo/components/ModelRendererSystem.h"

#include "origo/components/ModelRenderer.h"
#include "origo/components/Transform.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void ModelRenderSystem::Render(Scene* scene, RenderContext& context) {
	scene->View<ModelRendererComponent, TransformComponent>([&](RID, auto& mr, auto& tr) {
		if (!mr.ModelHandle.has_value())
			return;

		context.SubmitModel(*mr.ModelHandle, tr.GetModelMatrix());
	});
}
}

REGISTER_RENDER_SYSTEM(
    Origo::GamePhase::RenderAlways,
    Origo::ModelRenderSystem)
