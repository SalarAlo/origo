#pragma once

#include "origo/renderer/Shader.h"
#include "origo/renderer/UniformList.hpp"

namespace Origo {

class Material {
public:
	Material(Ref<UniformList> uniformList, Ref<Shader> shader);
	void Bind();

private:
	Ref<Shader> m_Shader;
	Ref<UniformList> m_UniformList;
};

}
