#include "origo/renderer/GeometryHeap.h"

namespace Origo {

GeometryHeap::GeometryHeap(int vertexLayoutId, GLenum usage, size_t maxVertexBytes, size_t maxIndexBytes)
    : m_VertexLayoutId(vertexLayoutId)
    , m_Usage(usage)
    , m_MaxVertexBytes(maxVertexBytes)
    , m_MaxIndexBytes(maxIndexBytes) {
	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, maxVertexBytes, nullptr, usage);

	glGenBuffers(1, &m_Ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndexBytes, nullptr, usage);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GeometryHeap::~GeometryHeap() {
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ibo);
}

MeshRange GeometryHeap::Allocate(
    const void* vertexData,
    size_t vertexBytes,
    size_t sizeOfSingleVertexInBytes,
    const unsigned int* indexData,
    size_t indexCount) {
	MeshRange range;

	range.FirstVertex = static_cast<uint32_t>(m_VertexWriteHeadBytes / sizeOfSingleVertexInBytes);
	range.VertexCount = static_cast<uint32_t>(vertexBytes / sizeOfSingleVertexInBytes);

	range.FirstIndex = static_cast<uint32_t>(m_IndexWriteHeadBytes / sizeof(unsigned int));
	range.IndexCount = static_cast<uint32_t>(indexCount);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferSubData(GL_ARRAY_BUFFER, m_VertexWriteHeadBytes, vertexBytes, vertexData);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_IndexWriteHeadBytes, indexCount * sizeof(unsigned int), indexData);

	m_VertexWriteHeadBytes += vertexBytes;
	m_IndexWriteHeadBytes += indexCount * sizeof(unsigned int);

	return range;
}

}
