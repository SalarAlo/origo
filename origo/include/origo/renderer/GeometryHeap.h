#pragma once

#include "origo/renderer/MeshRange.h"

namespace Origo {

class GeometryHeap {
public:
	GeometryHeap(int vertexLayoutId, GLenum usage, size_t maxVertexBytes, size_t maxIndexBytes);
	~GeometryHeap();

	MeshRange Allocate(
	    const void* vertexData,
	    size_t vertexBytes,
	    size_t vertexStrideBytes,
	    const unsigned int* indexData,
	    size_t indexCount);

	GLuint GetVbo() const { return m_Vbo; }
	GLuint GetIbo() const { return m_Ibo; }

private:
	int m_VertexLayoutId;

	GLuint m_Vbo = 0;
	GLuint m_Ibo = 0;

	GLenum m_Usage = GL_STATIC_DRAW;

	size_t m_MaxVertexBytes = 0;
	size_t m_MaxIndexBytes = 0;

	size_t m_VertexWriteHeadBytes = 0;
	size_t m_IndexWriteHeadBytes = 0;
};

}
