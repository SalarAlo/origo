#include "layers/SceneLayer.h"

#include "origo/assets/DefaultAssetCache.h"
#include "origo/assets/PrimitiveShapeCache.h"
#include "origo/assets/Shader.h"

#include "origo/components/CameraComponent.h"
#include "origo/components/DirectionalLight.h"
#include "origo/components/Transform.h"

#include "origo/core/Time.h"

using namespace Origo;

namespace OrigoEditor {

SceneLayer::SceneLayer(EditorContext& ctx)
    : m_context(ctx) {
}

void SceneLayer::on_attach() {
	auto scene { m_context.ActiveScene };
	auto cam { scene->create_entity("Main Camera") };

	auto camera_transform_component { scene->get_native_component<TransformComponent>(cam) };
	auto camera_component { scene->add_native_component<Origo::CameraComponent>(cam) };
	camera_component->IsPrimary = true;
	camera_transform_component->set_position({ -3.5, 4, 5 });
	camera_transform_component->set_rotation({ -33, -33, 0 });

	auto light_entity { scene->create_entity("Light") };
	auto light_transform_component { scene->get_native_component<TransformComponent>(light_entity) };
	auto light_component { scene->add_native_component<Origo::DirectionalLightComponent>(light_entity) };
	light_transform_component->set_position({ 4.0, 4.0, 3.0 });
	light_transform_component->look_at({ 0, 0, 0 });

	PrimitiveShapeCache::get_instance();
}

void SceneLayer::on_update(double) {
	auto shader = AssetManager::get_instance().get_asset<Shader>(DefaultAssetCache::get_instance().get_shader());

	shader->use_program();
	shader->set_uniform("u_Time",
	    static_cast<float>(Time::get_time_since_start().count()));
	shader->set_uniform(
	    "u_ScreenSize",
	    glm::vec2(
	        m_context.RenderBuffer.get_width(),
	        m_context.RenderBuffer.get_height()));
}

}
