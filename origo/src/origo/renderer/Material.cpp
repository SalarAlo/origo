#include "origo/renderer/Material.h"

namespace Origo {
Material::Material(Ref<UniformList> uniformList, Ref<Shader> shader)
    : m_UniformList(uniformList)
    , m_Shader(shader) {
}

void Material::Bind() {
	m_Shader->UseProgram();
	m_UniformList->UploadAll(m_Shader);
}
}
