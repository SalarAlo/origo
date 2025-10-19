
#include "origo/renderer/Shader.h"
#include "origo/core/Logger.h"

#include <glm/gtc/type_ptr.hpp>

namespace Origo {

static GLuint CompileShader(GLenum type, const char* src) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint len = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		std::string log(len, '\0');
		glGetShaderInfoLog(shader, len, &len, log.data());
		glDeleteShader(shader);
		ORG_CORE_ERROR("[Shader] Shader Compilation Failed!");
		throw std::runtime_error(std::string("Shader compile failed: ") + log);
	}

	return shader;
}

static GLuint LinkProgram(GLuint vs, GLuint fs) {
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	GLint linked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		std::string log(len, '\0');
		glGetProgramInfoLog(program, len, &len, log.data());
		glDeleteProgram(program);
		ORG_CORE_ERROR("[Shader] Program Linkage failed!");
		throw std::runtime_error(std::string("Program link failed: ") + log);
	}
	return program;
}

Shader::Shader(Ref<ShaderSource> src)
    : m_Source(src) {
	auto shaderData { src->GetShaderData() };
	auto vertexId { CompileShader(GL_VERTEX_SHADER, shaderData.VertexShader) };
	auto fragmentId { CompileShader(GL_FRAGMENT_SHADER, shaderData.FragmentShader) };
	m_ProgramId = LinkProgram(vertexId, fragmentId);
}

Shader::~Shader() {
	glDeleteProgram(m_ProgramId);
}

void Shader::UseProgram() const {
	glUseProgram(m_ProgramId);
}

#pragma region TEMPLATE_UNIFORM_SPECIALIZATIONS
template <>
void Shader::SetUniform<glm::mat4>(std::string_view name, const glm::mat4& mat) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

template <>
void Shader::SetUniform(std::string_view name, const glm::vec3& pos) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniform3f(loc, pos.x, pos.y, pos.z);
}

template <>
void Shader::SetUniform<float>(std::string_view name, const float& val) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniform1f(loc, val);
}

template <>
void Shader::SetUniform<glm::vec2>(std::string_view name, const glm::vec2& pos) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniform2f(loc, pos.x, pos.y);
}

template <>
void Shader::SetUniform<int>(std::string_view name, const int& value) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniform1i(loc, value);
}
#pragma endregion
}
