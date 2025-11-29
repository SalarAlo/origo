#include "origo/assets/Material.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Texture.h"
#include "origo/assets/UniformList.hpp"
#include "origo/core/Logger.h"

namespace Origo {
Material::Material(RID shader, RID material)
    : m_UniformList()
    , m_Shader(shader)
    , m_Albedo(material) {
	if (m_Albedo.IsNull())
		return;
	auto albedo { AssetManager::GetAssetAs<Texture>(m_Albedo) };
	if (albedo->GetTextureType() != TextureType::Albedo) {
		ORG_CORE_ERROR(
		    "[Material] Expected a Texture Type of Albedo. Received a Texture Type of {}",
		    magic_enum::enum_name(albedo->GetTextureType()));
	}
}

void Material::Bind() {
	auto shader { AssetManager::GetAssetAs<Shader>(m_Shader) };
	shader->UseProgram();
	m_UniformList.UploadAll(shader);

	if (m_Albedo.IsValid()) {
		auto albedo { AssetManager::GetAssetAs<Texture>(m_Albedo) };
		albedo->Bind(m_Shader);
	}
}

void Material::WriteModel(const glm::mat4& modelMatrix) {
	auto shader { AssetManager::GetAssetAs<Shader>(m_Shader) };
	shader->SetUniform("u_ModelMatrix", modelMatrix);
}
}
