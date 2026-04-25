#pragma once

namespace Origo {
class VertexArray {
public:
	VertexArray();
	~VertexArray();

	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;

	VertexArray(VertexArray&&) = delete;
	VertexArray& operator=(VertexArray&&) = delete;

	void bind() const;
	void unbind() const;

private:
	GLuint m_buffer_id {};
};
}
