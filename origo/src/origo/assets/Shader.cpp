
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"
#include "origo/core/Logger.h"

#include <glm/gtc/type_ptr.hpp>

namespace Origo {

static GLuint CompileShader(GLenum type, const char* src) {
	GLuint shader = glCreateShader(type);
	GLCall(glShaderSource(shader, 1, &src, nullptr));
	GLCall(glCompileShader(shader));
	GLint compiled = 0;
	GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
	if (!compiled) {
		GLint len = 0;
		GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len));
		std::string log(len, '\0');
		GLCall(glGetShaderInfoLog(shader, len, &len, log.data()));
		GLCall(glDeleteShader(shader));
		ORG_CORE_ERROR("[Shader] Shader Compilation Failed!");
		throw std::runtime_error(std::string("Shader compile failed: ") + log);
	}

	return shader;
}

static GLuint LinkProgram(GLuint vs, GLuint fs) {
	GLuint program = glCreateProgram();
	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLint linked = 0;
	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &linked));
	if (!linked) {
		GLint len = 0;
		GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len));
		std::string log(len, '\0');
		GLCall(glGetProgramInfoLog(program, len, &len, log.data()));
		GLCall(glDeleteProgram(program));
		ORG_CORE_ERROR("[Shader] Program Linkage failed!");
		throw std::runtime_error(std::string("Program link failed: ") + log);
	}
	return program;
}

Shader::Shader(std::string_view path)
    : m_Source(new ShaderSourceFile(path)) {
	Init();
}

Shader::Shader(std::string_view vertexShader, std::string_view fragmentShader)
    : m_Source(new ShaderSourceRaw(vertexShader, fragmentShader)) {
	Init();
}

void Shader::Init() {
	auto shaderData { m_Source->GetShaderData() };
	auto vertexId { CompileShader(GL_VERTEX_SHADER, shaderData.VertexShader) };
	auto fragmentId { CompileShader(GL_FRAGMENT_SHADER, shaderData.FragmentShader) };
	m_ProgramId = LinkProgram(vertexId, fragmentId);

	GLCall(glDetachShader(m_ProgramId, vertexId));
	GLCall(glDetachShader(m_ProgramId, fragmentId));
	GLCall(glDeleteShader(vertexId));
	GLCall(glDeleteShader(fragmentId));
}

Shader::~Shader() {
	if (m_ProgramId)
		GLCall(glDeleteProgram(m_ProgramId));
	delete m_Source;
}

void Shader::UseProgram() const {
	GLCall(glUseProgram(m_ProgramId));
}

#pragma region TEMPLATE_UNIFORM_SPECIALIZATIONS
template <>
void Shader::SetUniform<glm::mat4>(std::string_view name, const glm::mat4& mat) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	GLCall(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat)));
}

template <>
void Shader::SetUniform<glm::vec3>(std::string_view name, const glm::vec3& pos) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	GLCall(glUniform3f(loc, pos.x, pos.y, pos.z));
}

template <>
void Shader::SetUniform<float>(std::string_view name, const float& val) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	GLCall(glUniform1f(loc, val));
}

template <>
void Shader::SetUniform<glm::vec2>(std::string_view name, const glm::vec2& pos) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	GLCall(glUniform2f(loc, pos.x, pos.y));
}

template <>
void Shader::SetUniform<int>(std::string_view name, const int& value) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	GLCall(glUniform1i(loc, value));
}

template <>
void Shader::SetUniform<unsigned int>(std::string_view name, const unsigned int& value) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	GLCall(glUniform1ui(loc, value));
}

#pragma endregion
}
