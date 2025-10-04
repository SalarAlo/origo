#pragma once

#include <glm/glm.hpp>

class Shader {
public:
	Shader(std::string_view name);
	~Shader();
	void UseProgram() const;

	void SetUniformMat4(std::string_view name, const glm::mat4& matrix) const;

	void SetUniformVec2(std::string_view name, const glm::vec2& pos) const;
	void SetUniformVec3(std::string_view name, const glm::vec3& pos) const;

	void SetUniformInt(std::string_view name, int value) const;

private:
	unsigned int m_ProgramId {};
};
