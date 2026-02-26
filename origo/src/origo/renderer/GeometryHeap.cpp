#include "origo/renderer/GeometryHeap.h"

namespace Origo {

GeometryHeap::GeometryHeap(int vertexLayoutId, GLenum usage, size_t maxVertexBytes, size_t maxIndexBytes)
    : m_vertex_layout_id(vertexLayoutId)
    , m_usage(usage)
    , m_max_vertex_bytes(maxVertexBytes)
    , m_max_index_bytes(maxIndexBytes) {
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, maxVertexBytes, nullptr, usage);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndexBytes, nullptr, usage);
}

GeometryHeap::~GeometryHeap() {
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

MeshRange GeometryHeap::allocate(
    const void* vertexData,
    size_t vertexBytes,
    size_t sizeOfSingleVertexInBytes,
    const unsigned int* indexData,
    size_t indexCount) {
	MeshRange range;

	range.FirstVertex = static_cast<uint32_t>(m_vertex_write_head_bytes / sizeOfSingleVertexInBytes);
	range.VertexCount = static_cast<uint32_t>(vertexBytes / sizeOfSingleVertexInBytes);

	range.FirstIndex = static_cast<uint32_t>(m_index_write_head_bytes / sizeof(unsigned int));
	range.IndexCount = static_cast<uint32_t>(indexCount);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, m_vertex_write_head_bytes, vertexBytes, vertexData);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, m_index_write_head_bytes, indexCount * sizeof(unsigned int), indexData);

	m_vertex_write_head_bytes += vertexBytes;
	m_index_write_head_bytes += indexCount * sizeof(unsigned int);

	return range;
}

}
