#include "origo/assets/Material.h"
#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Texture2D.h"
#include "origo/assets/UniformList.hpp"
#include "origo/core/Logger.h"

namespace Origo {

Material2D::Material2D() {
	m_Shader = Shader::DefaultShader();
	m_Albedo = Texture2D::DefaultTexture();
}

Material2D::Material2D(AssetHandle shader, OptionalAssetHandle texture)
    : m_UniformList()
    , m_Shader(shader)
    , m_Albedo(texture) {
	if (!m_Albedo.has_value())
		return;

	auto albedoPtr { AssetManager::GetInstance().Get<Texture2D>(*m_Albedo) };
	if (albedoPtr->GetTextureType() != TextureType::Albedo) {
		ORG_CORE_ERROR(
		    "Material::Material: Expected a Texture Type of Albedo. Received a Texture Type of {}",
		    magic_enum::enum_name(albedoPtr->GetTextureType()));
	}
}

AssetHandle Material2D::DefaultMaterial2D() {
	static AssetHandle handle = [] {
		auto handle = AssetFactory::CreateSyntheticAsset<Material2D>("Default Material", UUID::FromArbitraryString("DEFAULT_MATERIAL_2D"));
		auto material { AssetManager::GetInstance().Get<Material2D>(handle) };

		material->SetShader(Shader::DefaultShader());
		material->SetAlbedo(Texture2D::DefaultTexture());
		return handle;
	}();

	return handle;
}

void Material2D::Bind() {
	auto& am { AssetManager::GetInstance() };
	auto shader { am.Get<Shader>(*m_Shader) };
	shader->UseProgram();
	m_UniformList.UploadAll(shader);

	if (m_Albedo.has_value()) {
		auto albedo { am.Get<Texture2D>(*m_Albedo) };
		albedo->Bind(*m_Shader);
	}
}

void Material2D::WriteModel(const glm::mat4& modelMatrix) {
	if (!m_Shader.has_value()) {
		ORG_TRACE("Material2D::WriteModel: Can not Write Model if no shader exists");
		return;
	}

	auto shader { AssetManager::GetInstance().Get<Shader>(*m_Shader) };
	shader->SetUniform("u_ModelMatrix", modelMatrix);
}

void Material2D::Resolve() {
	auto& am = AssetManager::GetInstance();

	m_Shader = !m_ShaderUUID.has_value() ? Shader::DefaultShader() : am.GetHandleByUUID(*m_ShaderUUID);
	if (!m_Shader.has_value()) {
		m_Shader = Shader::DefaultShader();
	}

	m_Albedo = !m_AlbedoUUID.has_value() ? Texture2D::DefaultTexture() : am.GetHandleByUUID(*m_AlbedoUUID);
	if (!m_Albedo.has_value()) {
		m_Albedo = Shader::DefaultShader();
	}
}

}
