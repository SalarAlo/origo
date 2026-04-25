#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <stdexcept>

#include "origo/assets/Shader.h"

#include "origo/assets/ShaderSource.h"

#include "origo/core/Logger.h"

#include "origo/renderer/GlDebug.h"

namespace Origo {

GLint Shader::get_uniform_location(std::string_view name) const {
	auto it = m_uniform_cache.find(std::string(name));
	if (it != m_uniform_cache.end())
		return it->second;

	GLint loc = glGetUniformLocation(m_program_id, name.data());
	m_uniform_cache.emplace(name, loc);
	return loc;
}

static GLuint compile_shader(GLenum type, const char* src) {
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

static GLuint link_program(GLuint vs, GLuint fs) {
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

void Shader::set_source(Scope<ShaderSource> source) {
	m_source = std::move(source);
	m_uniform_cache.clear();
}

void Shader::resolve() {
	unuse_program();
	glBindVertexArray(0);

	if (m_program_id) {
		GLCall(glDeleteProgram(m_program_id));
		m_program_id = 0;
	}

	m_uniform_cache.clear();

	auto shader_data { m_source->get_shader_data() };

	auto vertex_id { compile_shader(GL_VERTEX_SHADER, shader_data.VertexShader) };
	auto fragment_id { compile_shader(GL_FRAGMENT_SHADER, shader_data.FragmentShader) };

	m_program_id = link_program(vertex_id, fragment_id);

	GLCall(glDetachShader(m_program_id, vertex_id));
	GLCall(glDetachShader(m_program_id, fragment_id));
	GLCall(glDeleteShader(vertex_id));
	GLCall(glDeleteShader(fragment_id));
}

Shader::~Shader() {
	if (m_program_id)
		GLCall(glDeleteProgram(m_program_id));
}

void Shader::use_program() const {
	GLCall(glUseProgram(m_program_id));
}

void Shader::unuse_program() const {
	GLCall(glUseProgram(0));
}

#pragma region TEMPLATE_UNIFORM_SPECIALIZATIONS

template <>
void Shader::set_uniform<bool>(std::string_view name, const bool& value) const {
	GLint loc = get_uniform_location(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform1i(m_program_id, loc, value));
}

template <>
void Shader::set_uniform<glm::mat4>(
    std::string_view name, const glm::mat4& mat) const {
	GLint loc = get_uniform_location(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniformMatrix4fv(
	    m_program_id, loc, 1, GL_FALSE, glm::value_ptr(mat)));
}

template <>
void Shader::set_uniform<Vec3>(
    std::string_view name, const Vec3& v) const {
	GLint loc = get_uniform_location(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform3f(
	    m_program_id, loc, v.x, v.y, v.z));
}

template <>
void Shader::set_uniform<Vec2>(
    std::string_view name, const Vec2& v) const {
	GLint loc = get_uniform_location(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform2f(
	    m_program_id, loc, v.x, v.y));
}

template <>
void Shader::set_uniform<float>(
    std::string_view name, const float& v) const {
	GLint loc = get_uniform_location(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform1f(
	    m_program_id, loc, v));
}

template <>
void Shader::set_uniform<int>(
    std::string_view name, const int& v) const {
	GLint loc = get_uniform_location(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform1i(
	    m_program_id, loc, v));
}

template <>
void Shader::set_uniform<unsigned int>(
    std::string_view name, const unsigned int& v) const {
	GLint loc = get_uniform_location(name);
	if (loc == -1)
		return;

	GLCall(glProgramUniform1ui(
	    m_program_id, loc, v));
}
#pragma endregion
}
