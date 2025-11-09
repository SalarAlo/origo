#include "origo/assets/Material.h"
#include "origo/assets/UniformList.hpp"
#include "origo/core/Logger.h"

namespace Origo {
Material::Material(Shader* shader, Texture* material, bool shouldSerialize)
    : m_UniformList(new UniformList())
    , m_Shader(shader)
    , m_Albedo(material)
    , m_ShouldSerialize(shouldSerialize) {
	if (!m_Albedo)
		return;
	if (m_Albedo->GetTextureType() != TextureType::Albedo) {
		ORG_CORE_ERROR(
		    "[Material] Expected a Texture Type of Albedo. Received a Texture Type of {}",
		    magic_enum::enum_name(m_Albedo->GetTextureType()));
	}
}

Material::~Material() {
	delete m_UniformList;
}

void Material::Bind() {
	m_Shader->UseProgram();
	m_UniformList->UploadAll(m_Shader);

	if (m_Albedo)
		m_Albedo->Bind(m_Shader);
}

void Material::WriteModel(const glm::mat4& modelMatrix) {
	m_Shader->SetUniform("u_ModelMatrix", modelMatrix);
}
}
