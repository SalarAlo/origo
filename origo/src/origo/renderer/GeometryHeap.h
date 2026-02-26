#pragma once

#include "origo/renderer/MeshRange.h"

namespace Origo {

class GeometryHeap {
public:
	GeometryHeap(int vertexLayoutId, GLenum usage, size_t maxVertexBytes, size_t maxIndexBytes);
	~GeometryHeap();

	GeometryHeap(const GeometryHeap&) = delete;
	GeometryHeap& operator=(const GeometryHeap&) = delete;
	GeometryHeap(GeometryHeap&&) = delete;
	GeometryHeap& operator=(GeometryHeap&&) = delete;

	MeshRange allocate(
	    const void* vertexData,
	    size_t vertexBytes,
	    size_t vertexStrideBytes,
	    const unsigned int* indexData,
	    size_t indexCount);

	GLuint get_vbo() const { return m_vbo; }
	GLuint get_ibo() const { return m_ibo; }

private:
	int m_vertex_layout_id;

	GLuint m_vbo = 0;
	GLuint m_ibo = 0;

	GLenum m_usage = GL_STATIC_DRAW;

	size_t m_max_vertex_bytes = 0;
	size_t m_max_index_bytes = 0;

	size_t m_vertex_write_head_bytes = 0;
	size_t m_index_write_head_bytes = 0;
};

}
