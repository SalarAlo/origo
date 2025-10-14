#include "origo/renderer/Material.h"

namespace Origo {
Material::Material(const Ref<Shader>& shader)
    : m_UniformList(MakeRef<UniformList>())
    , m_Shader(shader) {
}

void Material::Bind() {
	m_Shader->UseProgram();
	m_UniformList->UploadAll(m_Shader);
}

void Material::WriteModel(const glm::mat4& modelMatrix) {
	m_Shader->SetUniform("u_ModelMatrix", modelMatrix);
}
}
