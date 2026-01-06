#include "origo/assets/Material.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/UniformList.hpp"
#include "origo/core/Logger.h"

namespace Origo {

Material2D::Material2D(AssetHandle shader, AssetHandle albedo)
    : m_UniformList()
    , m_Shader(shader)
    , m_Albedo(albedo) {
	if (m_Albedo.IsNull())
		return;

	auto albedoPtr { AssetManager::GetInstance().Get<Texture2D>(m_Albedo) };
	if (albedoPtr->GetTextureType() != TextureType::Albedo) {
		ORG_CORE_ERROR(
		    "Material::Material: Expected a Texture Type of Albedo. Received a Texture Type of {}",
		    magic_enum::enum_name(albedoPtr->GetTextureType()));
	}
}

AssetHandle Material2D::Default() {
	static auto material {
		[] {
		        return AssetFactory::CreateAsset<Material2D>("Default Material", Shader::DefaultNormalShader(), Texture2D::DefaultWhite());
		}()
	};
	return material;
}

void Material2D::Bind() {
	auto& am { AssetManager::GetInstance() };
	auto shader { am.Get<Shader>(m_Shader) };
	shader->UseProgram();
	m_UniformList.UploadAll(shader);

	if (!m_Albedo.IsNull()) {
		auto albedo { am.Get<Texture2D>(m_Albedo) };
		albedo->Bind(m_Shader);
	}
}

void Material2D::WriteModel(const glm::mat4& modelMatrix) {
	auto shader { AssetManager::GetInstance().Get<Shader>(m_Shader) };
	shader->SetUniform("u_ModelMatrix", modelMatrix);
}

}
