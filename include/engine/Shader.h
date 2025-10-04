#pragma once

#include <glm/glm.hpp>

class Shader {
public:
	Shader(std::string_view name);
	~Shader();
	void UseProgram() const;
	void SetUniformMat4(std::string_view name, const glm::mat4& matrix) const;
	void SetUniformVec3(std::string_view name, const glm::vec3& matrix) const;

private:
	unsigned int m_ProgramId {};
};
