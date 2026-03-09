#include "origo/components/systems/BoxColliderDebugGizmoSystem.h"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/BoxColliderComponent.h"
#include "origo/components/Transform.h"

#include "origo/renderer/RenderCommand.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

void BoxColliderDebugGizmoSystem::render(Scene* scene, RenderContext& render_ctx) {
	scene->view<BoxColliderComponent, TransformComponent>([&](RID entity, BoxColliderComponent& _, TransformComponent& transf) {
		render_ctx.submit_mesh(
		    PrimitiveShapeCache::get_instance().get_wire_cube_mesh(),
		    DefaultAssetCache::get_instance().get_debug_material(),
		    transf.get_model_matrix(),
		    RenderPass::Geometry,
		    GL_LINES);
	});
}

}

REGISTER_RENDER_SYSTEM(Origo::GamePhase::RenderEditor, Origo::BoxColliderDebugGizmoSystem);
