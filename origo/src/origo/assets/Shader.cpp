#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <stdexcept>

#include "origo/assets/Shader.h"

#include "origo/assets/ShaderSource.h"

#include "origo/core/Logger.h"

#include "origo/renderer/GlDebug.h"

namespace Origo {

GLint Shader::GetUniformLocation(std::string_view name) const {
	auto it = m_UniformCache.find(std::string(name));
	if (it != m_UniformCache.end())
		return it->second;

	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	m_UniformCache.emplace(name, loc);
	return loc;
}

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
		ORG_CORE_ERROR("Shader::CompileShader: Shader Compilation Failed!");
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
		ORG_CORE_ERROR("Shader::LinkProgram: Program Linkage failed!");
		throw std::runtime_error(std::string("Program link failed: ") + log);
	}
	return program;
}

void Shader::SetSource(Scope<ShaderSource> source) {
	m_Source = std::move(source);
	m_UniformCache.clear();
}

void Shader::Resolve() {
	UnuseProgram();
	glBindVertexArray(0);

	if (m_ProgramId) {
		GLCall(glDeleteProgram(m_ProgramId));
		m_ProgramId = 0;
	}

	m_UniformCache.clear();

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
}

void Shader::UseProgram() const {
	GLCall(glUseProgram(m_ProgramId));
}

void Shader::UnuseProgram() const {
	GLCall(glUseProgram(0));
}

#pragma region TEMPLATE_UNIFORM_SPECIALIZATIONS

template <>
void Shader::SetUniform<bool>(std::string_view name, const bool& value) const {
	GLint loc = GetUniformLocation(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform1i(m_ProgramId, loc, value));
}

template <>
void Shader::SetUniform<glm::mat4>(
    std::string_view name, const glm::mat4& mat) const {
	GLint loc = GetUniformLocation(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniformMatrix4fv(
	    m_ProgramId, loc, 1, GL_FALSE, glm::value_ptr(mat)));
}

template <>
void Shader::SetUniform<Vec3>(
    std::string_view name, const Vec3& v) const {
	GLint loc = GetUniformLocation(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform3f(
	    m_ProgramId, loc, v.x, v.y, v.z));
}

template <>
void Shader::SetUniform<Vec2>(
    std::string_view name, const Vec2& v) const {
	GLint loc = GetUniformLocation(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform2f(
	    m_ProgramId, loc, v.x, v.y));
}

template <>
void Shader::SetUniform<float>(
    std::string_view name, const float& v) const {
	GLint loc = GetUniformLocation(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform1f(
	    m_ProgramId, loc, v));
}

template <>
void Shader::SetUniform<int>(
    std::string_view name, const int& v) const {
	GLint loc = GetUniformLocation(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform1i(
	    m_ProgramId, loc, v));
}

template <>
void Shader::SetUniform<unsigned int>(
    std::string_view name, const unsigned int& v) const {
	GLint loc = GetUniformLocation(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform1ui(
	    m_ProgramId, loc, v));
}
#pragma endregion
}
