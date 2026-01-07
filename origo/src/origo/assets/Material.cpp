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

// fails because not UUID changes each time so that cant be stored
// as a ref in a .import file.
// 2 solutions:
// introduce a new type of ref thats not uuid but "name" for example
// make the default material a file and hook that up somehow in here
AssetHandle Material2D::DefaultMaterial2D() {
	static auto material {
		[] {
		        return AssetFactory::CreateAsset<Material2D>("Default Material", Shader::DefaultShader(), Texture2D::DefaultTexture());
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

void Material2D::Resolve() {
	auto& am = AssetManager::GetInstance();

	m_Shader = m_ShaderUUID.IsBad() ? Shader::DefaultShader() : am.GetHandleByUUID(m_ShaderUUID);
	if (m_Shader.IsNull()) {
		m_Shader = Shader::DefaultShader();
	}

	m_Albedo = m_AlbedoUUID.IsBad() ? Texture2D::DefaultTexture() : am.GetHandleByUUID(m_AlbedoUUID);
	if (m_Albedo.IsNull()) {
		m_Albedo = Shader::DefaultShader();
	}
}

}
