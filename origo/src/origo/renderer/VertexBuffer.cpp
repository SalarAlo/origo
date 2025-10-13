#include "origo/renderer/VertexBuffer.h"

namespace Origo {
VertexBuffer::VertexBuffer(const std::vector<float>& data)
    : m_Data(data) {
	glGenBuffers(1, &m_BufferId);
	SetDataOpenGL(true);
}

void VertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
}

void VertexBuffer::Unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::AddData(const std::vector<float>& data) {
	m_Data.insert(m_Data.end(), data.begin(), data.end());
	SetDataOpenGL(false);
}
void VertexBuffer::ReplaceData(const std::vector<float>& data) {
	m_Data = data;
	SetDataOpenGL(false);
}

void VertexBuffer::SetDataOpenGL(bool initialUpload) const {
	Bind();

	static size_t previousSize = 0;
	const size_t currentSize = m_Data.size() * sizeof(float);

	if (initialUpload || currentSize > previousSize) {
		glBufferData(GL_ARRAY_BUFFER, currentSize, m_Data.data(), GL_DYNAMIC_DRAW);
		previousSize = currentSize;
	} else {
		glBufferSubData(GL_ARRAY_BUFFER, 0, currentSize, m_Data.data());
	}

	Unbind();
}

}
