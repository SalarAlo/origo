#include "origo/renderer/VertexBuffer.h"

namespace Origo {
VertexBuffer::VertexBuffer(const std::vector<float>& data)
    : m_Data(data) {
	glGenBuffers(1, &m_BufferId);
	SetDataOpenGL();
}

void VertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
}

void VertexBuffer::Unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::AddData(const std::vector<float>& data) {
	m_Data.insert(m_Data.end(), m_Data.begin(), m_Data.end());
	SetDataOpenGL();
}
void VertexBuffer::ReplaceData(const std::vector<float>& data) {
	m_Data = data;
	SetDataOpenGL();
}

void VertexBuffer::SetDataOpenGL() const {
	bool initiallyBound = m_Bound;
	Bind();
	glBufferData(GL_ARRAY_BUFFER, m_Data.size() * sizeof(float), m_Data.data(), GL_DYNAMIC_DRAW);
	if (!initiallyBound)
		Unbind();
}
}
