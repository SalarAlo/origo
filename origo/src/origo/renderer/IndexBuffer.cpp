#include "origo/renderer/IndexBuffer.h"

namespace Origo {
IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices)
    : m_Data(indices) {
	GLCall(glGenBuffers(1, &m_BufferId));
	SetDataOpenGL();
};

void IndexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId));
	s_CurrentlyBound = m_BufferId;
}

void IndexBuffer::Unbind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	s_CurrentlyBound = 0;
}

void IndexBuffer::BindTemp() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId));
}

void IndexBuffer::UnbindTemp() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_CurrentlyBound));
}

std::size_t IndexBuffer::GetElementCount() const {
	return m_Data.size();
}

void IndexBuffer::AddData(const std::vector<unsigned int>& data) {
	m_Data.insert(m_Data.end(), data.begin(), data.end());
	SetDataOpenGL();
}
void IndexBuffer::ReplaceData(const std::vector<unsigned int>& data) {
	m_Data = data;
	SetDataOpenGL();
}

void IndexBuffer::SetDataOpenGL() {
	BindTemp();
	size_t bytes = m_Data.size() * sizeof(unsigned int);
	if (bytes > m_Capacity) {
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, m_Data.data(), GL_DYNAMIC_DRAW));
		m_Capacity = bytes;
	} else {
		GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, bytes, m_Data.data()));
	}
	UnbindTemp();
}
}
