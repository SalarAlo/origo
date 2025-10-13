#pragma once

#include "origo/renderer/Shader.h"
#include "origo/renderer/UniformList.hpp"

namespace Origo {

class Material {
public:
	Material(const Ref<UniformList>& uniformList, const Ref<Shader>& shader);
	void Bind();
	void WriteModel(const glm::mat4& model);

private:
	Ref<Shader> m_Shader;
	Ref<UniformList> m_UniformList;
};

}
