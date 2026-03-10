#include "origo/components/systems/BoxColliderDebugGizmoSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"

#include "origo/components/BoxColliderComponent.h"
#include "origo/components/Transform.h"

#include "origo/renderer/RenderCommand.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

namespace Origo {

namespace {

glm::mat4 get_collider_matrix(const TransformComponent& transform, const BoxColliderComponent& collider) {
	glm::mat4 model(1.0f);

	model = glm::translate(model, transform.get_position());
	model *= glm::toMat4(glm::quat(glm::radians(transform.get_rotation())));
	model = glm::translate(model, transform.get_scale() * collider.Offset);
	model = glm::scale(model, transform.get_scale() * collider.Size);

	return model;
}

}

void BoxColliderDebugGizmoSystem::render(Scene* scene, RenderContext& render_ctx) {
	scene->view<BoxColliderComponent, TransformComponent>([&](RID entity, BoxColliderComponent& collider, TransformComponent& transf) {
		render_ctx.submit_mesh(
		    PrimitiveShapeCache::get_instance().get_wire_cube_mesh(),
		    DefaultAssetCache::get_instance().get_debug_material(),
		    get_collider_matrix(transf, collider),
		    RenderPass::Geometry,
		    GL_LINES);
	});
}

}

REGISTER_RENDER_SYSTEM(Origo::GamePhase::RenderEditor, Origo::BoxColliderDebugGizmoSystem);
