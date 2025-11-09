#include "origo/renderer/VertexBuffer.h"

namespace Origo {

VertexBuffer::VertexBuffer(const std::vector<float>& data)
    : m_Data(data) {
	GLCall(glGenBuffers(1, &m_BufferId));
	SetDataOpenGL(true);
}

VertexBuffer::~VertexBuffer() {
	GLCall(glDeleteBuffers(1, &m_BufferId));
}

void VertexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_BufferId));
	s_CurrentlyBound = m_BufferId;
}

void VertexBuffer::BindTemp() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_BufferId));
}
void VertexBuffer::UnbindTemp() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, s_CurrentlyBound));
}

void VertexBuffer::Unbind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	s_CurrentlyBound = 0;
}

void VertexBuffer::AddData(const std::vector<float>& data) {
	m_Data.insert(m_Data.end(), data.begin(), data.end());
	SetDataOpenGL(false);
}
void VertexBuffer::ReplaceData(const std::vector<float>& data) {
	m_Data = data;
	SetDataOpenGL(false);
}

void VertexBuffer::SetDataOpenGL(bool initialUpload) {
	BindTemp();

	const size_t currentSize = m_Data.size() * sizeof(float);

	if (initialUpload || currentSize > m_Capacity) {
		GLCall(glBufferData(GL_ARRAY_BUFFER, currentSize, m_Data.data(), GL_DYNAMIC_DRAW));
		m_Capacity = currentSize;
	} else {
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, currentSize, m_Data.data()));
	}

	UnbindTemp();
}

}
