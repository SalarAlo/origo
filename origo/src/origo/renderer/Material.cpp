#include "origo/renderer/Material.h"
#include "origo/core/Logger.h"

namespace Origo {
Material::Material(const Ref<Shader>& shader, const Ref<Texture>& albedo)
    : m_UniformList(MakeRef<UniformList>())
    , m_Shader(shader)
    , m_Albedo(albedo) {
	if (!m_Albedo)
		return;
	if (m_Albedo->GetType() != TextureType::Albedo) {
		ORG_CORE_ERROR(
		    "Expected a Texture Type of Albedo. Received a Texture Type of {}",
		    magic_enum::enum_name(m_Albedo->GetType()));
	}
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
