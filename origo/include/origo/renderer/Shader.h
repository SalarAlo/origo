#pragma once

#include <glm/glm.hpp>

namespace Origo {
class Shader {
public:
	Shader(std::string_view name);
	Shader(std::string_view vertShader, std::string_view fragShader);
	~Shader();
	void UseProgram() const;

	template <typename T>
	void SetUniform(std::string_view name, const T& value) const;

private:
	unsigned int m_ProgramId {};
};
}
