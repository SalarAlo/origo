#include "origo/renderer/IndexBuffer.h"
#include "origo/renderer/GlDebug.h"

namespace Origo {

IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices)
    : m_size(indices.size())
    , m_capacity(m_size) {
	m_data = static_cast<unsigned int*>(operator new(m_capacity * sizeof(unsigned int)));
	memcpy(m_data, indices.data(), m_size * sizeof(unsigned int));

	GLCall(glGenBuffers(1, &m_buffer_id));
	upload_initial();
}

IndexBuffer::~IndexBuffer() {
	GLCall(glDeleteBuffers(1, &m_buffer_id));
	operator delete(m_data);
}

void IndexBuffer::bind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_id));
	s_currently_bound = m_buffer_id;
}

void IndexBuffer::unbind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	s_currently_bound = 0;
}

void IndexBuffer::add_data(const std::vector<unsigned int>& data) {
	m_old_size = m_size;
	size_t new_size = m_size + data.size();

	if (new_size > m_capacity) {
		size_t new_capacity = std::max(m_capacity * 2, new_size);
		unsigned int* new_block = static_cast<unsigned int*>(operator new(new_capacity * sizeof(unsigned int)));

		memcpy(new_block, m_data, m_size * sizeof(unsigned int));
		operator delete(m_data);

		m_data = new_block;
		m_capacity = new_capacity;
	}

	memcpy(m_data + m_size, data.data(), data.size() * sizeof(unsigned int));
	m_size = new_size;

	bind();

	if (new_size > m_capacity) {
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		    m_size * sizeof(unsigned int),
		    m_data,
		    GL_DYNAMIC_DRAW));
	} else {
		upload_incremental(m_old_size);
	}
}

void IndexBuffer::upload_initial() {
	bind();
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	    m_size * sizeof(unsigned int),
	    m_data,
	    GL_DYNAMIC_DRAW));
}

void IndexBuffer::upload_incremental(size_t oldSize) {
	const size_t added = m_size - oldSize;

	GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
	    oldSize * sizeof(unsigned int),
	    added * sizeof(unsigned int),
	    m_data + oldSize));
}

}
