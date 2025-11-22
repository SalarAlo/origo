#include "origo/renderer/VertexBuffer.h"

namespace Origo {

VertexBuffer::VertexBuffer(const std::vector<float>& data)
    : m_Size(data.size() * sizeof(float))
    , m_Capacity(m_Size) {
	m_Data = operator new(m_Capacity);
	memcpy(m_Data, data.data(), m_Size);

	GLCall(glGenBuffers(1, &m_BufferId));
	SetDataOpenGL(true);
}

VertexBuffer::~VertexBuffer() {
	GLCall(glDeleteBuffers(1, &m_BufferId));
	operator delete(m_Data);
}

void VertexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_BufferId));
	s_CurrentlyBound = m_BufferId;
}

void VertexBuffer::Unbind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	s_CurrentlyBound = 0;
}

void VertexBuffer::AddData(const void* data, size_t bytes) {
	m_SizeBeforeLastAppend = m_Size;
	size_t newSize = m_Size + bytes;

	if (newSize > m_Capacity) {
		Resize(newSize);
		memcpy(static_cast<char*>(m_Data) + m_Size, data, bytes);

		Bind();

		GLCall(glBufferData(GL_ARRAY_BUFFER, m_Capacity, nullptr, GL_DYNAMIC_DRAW));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, newSize, m_Data));

		Unbind();
	} else {
		memcpy(static_cast<char*>(m_Data) + m_Size, data, bytes);

		Bind();
		GLCall(glBufferSubData(GL_ARRAY_BUFFER,
		    m_SizeBeforeLastAppend,
		    bytes,
		    static_cast<const char*>(data)));
		Unbind();
	}

	m_Size = newSize;
}

void VertexBuffer::Resize(size_t newSize) {
	size_t newCapacity = std::max(m_Capacity * 2, newSize);
	void* newBlock = operator new(newCapacity);

	memcpy(newBlock, m_Data, m_Size);
	operator delete(m_Data);

	m_Data = newBlock;
	m_Capacity = newCapacity;
}

void VertexBuffer::SetDataOpenGL(bool initialUpload) {
	Bind();

	if (initialUpload) {
		GLCall(glBufferData(GL_ARRAY_BUFFER, m_Capacity, nullptr, GL_DYNAMIC_DRAW));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, m_Size, m_Data));
	} else {
		GLCall(glBufferSubData(GL_ARRAY_BUFFER,
		    m_SizeBeforeLastAppend,
		    m_Size - m_SizeBeforeLastAppend,
		    static_cast<char*>(m_Data) + m_SizeBeforeLastAppend));
	}

	Unbind();
}

}
