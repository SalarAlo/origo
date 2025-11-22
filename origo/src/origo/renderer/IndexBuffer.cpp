#include "origo/renderer/IndexBuffer.h"
#include "origo/renderer/GlDebug.h"

namespace Origo {

IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices)
    : m_Size(indices.size())
    , m_Capacity(m_Size) {
	m_Data = static_cast<unsigned int*>(operator new(m_Capacity * sizeof(unsigned int)));
	memcpy(m_Data, indices.data(), m_Size * sizeof(unsigned int));

	GLCall(glGenBuffers(1, &m_BufferId));
	UploadInitial();
}

IndexBuffer::~IndexBuffer() {
	GLCall(glDeleteBuffers(1, &m_BufferId));
	operator delete(m_Data);
}

void IndexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId));
	s_CurrentlyBound = m_BufferId;
}

void IndexBuffer::Unbind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	s_CurrentlyBound = 0;
}

void IndexBuffer::AddData(const std::vector<unsigned int>& data) {
	m_OldSize = m_Size;
	size_t newSize = m_Size + data.size();

	if (newSize > m_Capacity) {
		size_t newCapacity = std::max(m_Capacity * 2, newSize);
		unsigned int* newBlock = static_cast<unsigned int*>(operator new(newCapacity * sizeof(unsigned int)));

		memcpy(newBlock, m_Data, m_Size * sizeof(unsigned int));
		operator delete(m_Data);

		m_Data = newBlock;
		m_Capacity = newCapacity;
	}

	memcpy(m_Data + m_Size, data.data(), data.size() * sizeof(unsigned int));
	m_Size = newSize;

	Bind();

	if (newSize > m_Capacity) {
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		    m_Size * sizeof(unsigned int),
		    m_Data,
		    GL_DYNAMIC_DRAW));
	} else {
		UploadIncremental(m_OldSize);
	}
}

void IndexBuffer::UploadInitial() {
	Bind();
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	    m_Size * sizeof(unsigned int),
	    m_Data,
	    GL_DYNAMIC_DRAW));
}

void IndexBuffer::UploadIncremental(size_t oldSize) {
	const size_t added = m_Size - oldSize;

	GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
	    oldSize * sizeof(unsigned int),
	    added * sizeof(unsigned int),
	    m_Data + oldSize));
}

}
