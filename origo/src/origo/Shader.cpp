
#include "origo/Shader.h"

#include <glm/gtc/type_ptr.hpp>

namespace {
struct ShaderData {
	const char* VertexShader {};
	const char* FragmentShader {};
};
}
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
		throw std::runtime_error(std::string("Shader compile failed: ") + log);
		std::cerr << "Starting the Program\n";
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
		throw std::runtime_error(std::string("Program link failed: ") + log);
	}
	return program;
}

static std::string ReadFile(std::string_view path) {
	constexpr std::size_t readSize { 4096 };
	auto stream = std::ifstream(path.data());
	if (!stream) {
		throw std::ios_base::failure("file does not exist");
	}
	std::string out {};
	std::string buf(readSize, '\0');
	while (stream.read(&buf[0], readSize)) {
		out.append(buf, 0, stream.gcount());
	}
	out.append(buf, 0, stream.gcount());
	return out;
}

static ShaderData GetData(std::string_view name) {
	ShaderData data {};
	std::string fileContent { ReadFile("./resources/shaders/" + std::string(name) + ".glsl") };

	auto vertPos { fileContent.find("#VERTEX") };
	auto fragPos { fileContent.find("#FRAGMENT") };

	if (vertPos == std::string::npos || fragPos == std::string::npos) {
		throw std::runtime_error("Shader file missing #VERTEX or #FRAGMENT section");
	}

	std::string vertexSrc { fileContent.substr(vertPos + 7, fragPos - (vertPos + 7)) };
	std::string fragmentSrc { fileContent.substr(fragPos + 9) };

	static std::string vertexStorage, fragmentStorage;
	vertexStorage = std::move(vertexSrc);
	fragmentStorage = std::move(fragmentSrc);

	data.VertexShader = vertexStorage.c_str();
	data.FragmentShader = fragmentStorage.c_str();

	return data;
}

Shader::Shader(std::string_view name) {
	auto shaderData { GetData(name) };
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

void Shader::SetUniformMat4(std::string_view name, const glm::mat4& mat) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniformVec3(std::string_view name, const glm::vec3& pos) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniform3f(loc, pos.x, pos.y, pos.z);
}

void Shader::SetUniformFloat(std::string_view name, float val) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniform1f(loc, val);
}

void Shader::SetUniformVec2(std::string_view name, const glm::vec2& pos) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniform2f(loc, pos.x, pos.y);
}

void Shader::SetUniformInt(std::string_view name, int value) const {
	GLint loc = glGetUniformLocation(m_ProgramId, name.data());
	if (loc == -1) {
		return;
	}
	glUniform1i(loc, value);
}
}
