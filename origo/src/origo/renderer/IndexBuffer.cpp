#include "origo/renderer/IndexBuffer.h"

namespace Origo {
IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices)
    : m_ElementCount(indices.size()) {
	glGenBuffers(1, &m_BufferId);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
};

void IndexBuffer::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
}

void IndexBuffer::Unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::size_t IndexBuffer::GetElementCount() const {
	return m_ElementCount;
}
}
