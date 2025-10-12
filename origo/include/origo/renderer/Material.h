#pragma once

#include "origo/renderer/Shader.h"
#include "origo/renderer/UniformList.hpp"

namespace Origo {

class Material {
public:
	Material(std::shared_ptr<UniformList> uniformList, std::shared_ptr<Shader> shader);

private:
	std::shared_ptr<Shader> m_Shader;
	std::shared_ptr<UniformList> m_UniformList;
};

}
