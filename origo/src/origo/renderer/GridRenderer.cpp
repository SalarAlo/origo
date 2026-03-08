#include "origo/renderer/GridRenderer.h"

#include "origo/renderer/GlDebug.h"
#include "origo/renderer/Helpers.h"

namespace Origo {

namespace {

	static std::string load_shader_source(const std::filesystem::path& primary_path, const std::filesystem::path& fallback_path) {
		try {
			return read_file(primary_path);
		} catch (...) {
			return read_file(fallback_path);
		}
	}

	static GLuint compile_shader(GLenum type, const std::string& source) {
		GLuint shader = glCreateShader(type);
		const char* src_ptr = source.c_str();
		GLCall(glShaderSource(shader, 1, &src_ptr, nullptr));
		GLCall(glCompileShader(shader));

		GLint compiled = 0;
		GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
		if (!compiled) {
			GLint len = 0;
			GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len));
			std::string log(len, '\0');
			GLCall(glGetShaderInfoLog(shader, len, &len, log.data()));
			GLCall(glDeleteShader(shader));
			throw std::runtime_error(std::string("Grid shader compile failed: ") + log);
		}

		return shader;
	}

	static GLuint create_grid_program() {
		const std::string grid_vertex_src = load_shader_source(
		    "./default_resources/grid.vert",
		    "./editor/workspace/default_resources/grid.vert");
		const std::string grid_fragment_src = load_shader_source(
		    "./default_resources/grid.frag",
		    "./editor/workspace/default_resources/grid.frag");

		GLuint vertex = compile_shader(GL_VERTEX_SHADER, grid_vertex_src);
		GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, grid_fragment_src);

		GLuint program = glCreateProgram();
		GLCall(glAttachShader(program, vertex));
		GLCall(glAttachShader(program, fragment));
		GLCall(glLinkProgram(program));

		GLint linked = 0;
		GLCall(glGetProgramiv(program, GL_LINK_STATUS, &linked));
		if (!linked) {
			GLint len = 0;
			GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len));
			std::string log(len, '\0');
			GLCall(glGetProgramInfoLog(program, len, &len, log.data()));

			GLCall(glDeleteShader(vertex));
			GLCall(glDeleteShader(fragment));
			GLCall(glDeleteProgram(program));
			throw std::runtime_error(std::string("Grid shader link failed: ") + log);
		}

		GLCall(glDetachShader(program, vertex));
		GLCall(glDetachShader(program, fragment));
		GLCall(glDeleteShader(vertex));
		GLCall(glDeleteShader(fragment));

		return program;
	}

}

GridRenderer::~GridRenderer() {
	if (m_program != 0) {
		GLCall(glDeleteProgram(m_program));
		m_program = 0;
	}
	if (m_fullscreen_vao != 0) {
		GLCall(glDeleteVertexArrays(1, &m_fullscreen_vao));
		m_fullscreen_vao = 0;
	}
}

void GridRenderer::initialize() {
	if (m_program == 0)
		m_program = create_grid_program();

	if (m_fullscreen_vao == 0)
		GLCall(glGenVertexArrays(1, &m_fullscreen_vao));
}

void GridRenderer::render(const RenderView& view) const {
	if (m_program == 0 || m_fullscreen_vao == 0)
		return;

	const glm::mat4 inv_view_proj = glm::inverse(view.Projection * view.View);

	GLCall(glUseProgram(m_program));
	GLCall(glProgramUniformMatrix4fv(m_program, glGetUniformLocation(m_program, "u_inv_view_proj"), 1, GL_FALSE, glm::value_ptr(inv_view_proj)));
	GLCall(glBindVertexArray(m_fullscreen_vao));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));
}

}
