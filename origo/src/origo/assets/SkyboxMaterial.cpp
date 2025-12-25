#include "origo/assets/SkyboxMaterial.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/CubemapTexture.h"
#include "origo/assets/Shader.h"

namespace Origo {

SkyboxMaterial::SkyboxMaterial(AssetHandle shader, AssetHandle cubemap)
    : m_Shader(shader)
    , m_Cubemap(cubemap) { }

void SkyboxMaterial::Bind(const glm::mat4& projection, const glm::mat4& view) {
	auto& am = AssetManagerFast::GetInstance();

	auto shader = am.Get<Shader>(m_Shader);
	shader->UseProgram();

	glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));

	shader->SetUniform("u_ProjectionMatrix", projection);
	shader->SetUniform("u_ViewMatrix", viewNoTranslation);

	auto cubemap = am.Get<CubemapTexture>(m_Cubemap);
	cubemap->Bind(0);

	shader->SetUniform("skybox", 0);
}

}
