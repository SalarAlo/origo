#include "origo/assets/SkyboxMaterial.h"

#include "origo/assets/AssetManager.h"
#include "origo/assets/CubemapTexture.h"
#include "origo/assets/Shader.h"

namespace Origo {

SkyboxMaterial::SkyboxMaterial(AssetHandle shader, AssetHandle cubemap)
    : m_shader(shader)
    , m_cubemap(cubemap) { }

void SkyboxMaterial::bind(const glm::mat4& projection, const glm::mat4& view) {
	auto& am = AssetManager::get_instance();

	auto shader = am.get_asset<Shader>(m_shader);
	shader->use_program();

	glm::mat4 view_no_translation = glm::mat4(glm::mat3(view));

	shader->set_uniform("u_ProjectionMatrix", projection);
	shader->set_uniform("u_ViewMatrix", view_no_translation);

	auto cubemap = am.get_asset<CubemapTexture>(m_cubemap);
	cubemap->bind(0);

	shader->set_uniform("skybox", 0);
}

}
