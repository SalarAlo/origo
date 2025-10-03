#include "engine/VertexBuffer.h"

VertexBuffer::VertexBuffer(std::vector<float> data) {
	glGenBuffers(1, &m_BufferId);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	Unbind();
}

void VertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
}

void VertexBuffer::Unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
