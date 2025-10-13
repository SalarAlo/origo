#include "origo/renderer/IndexBuffer.h"

namespace Origo {
IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices)
    : m_Data(indices) {
	glGenBuffers(1, &m_BufferId);
	SetDataOpenGL(true);
};

void IndexBuffer::Bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
}

void IndexBuffer::Unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

std::size_t IndexBuffer::GetElementCount() const {
	return m_Data.size();
}

void IndexBuffer::AddData(const std::vector<unsigned int>& data) {
	m_Data.insert(m_Data.end(), data.begin(), data.end());
	SetDataOpenGL(false);
}
void IndexBuffer::ReplaceData(const std::vector<unsigned int>& data) {
	m_Data = data;
	SetDataOpenGL(false);
}

void IndexBuffer::SetDataOpenGL(bool) const {
	Bind();
	static size_t capacity = 0;
	size_t bytes = m_Data.size() * sizeof(unsigned int);
	if (bytes > capacity) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, m_Data.data(), GL_DYNAMIC_DRAW);
		capacity = bytes;
	} else {
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bytes, m_Data.data());
	}
	Unbind();
}

}
