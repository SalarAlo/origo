#include "origo/renderer/VertexBuffer.h"

namespace Origo {

VertexBuffer::VertexBuffer(const std::vector<float>& data)
    : m_size(data.size() * sizeof(float))
    , m_capacity(m_size) {
	m_data = operator new(m_capacity);
	memcpy(m_data, data.data(), m_size);

	GLCall(glGenBuffers(1, &m_buffer_id));
	set_data_open_gl(true);
}

VertexBuffer::~VertexBuffer() {
	GLCall(glDeleteBuffers(1, &m_buffer_id));
	operator delete(m_data);
}

void VertexBuffer::bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id));
	s_currently_bound = m_buffer_id;
}

void VertexBuffer::unbind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	s_currently_bound = 0;
}

void VertexBuffer::add_data(const void* data, size_t bytes) {
	m_size_before_last_append = m_size;
	size_t new_size = m_size + bytes;

	if (new_size > m_capacity) {
		resize(new_size);
		memcpy(static_cast<char*>(m_data) + m_size, data, bytes);

		bind();

		GLCall(glBufferData(GL_ARRAY_BUFFER, m_capacity, nullptr, GL_DYNAMIC_DRAW));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, new_size, m_data));

		unbind();
	} else {
		memcpy(static_cast<char*>(m_data) + m_size, data, bytes);

		bind();
		GLCall(glBufferSubData(GL_ARRAY_BUFFER,
		    m_size_before_last_append,
		    bytes,
		    static_cast<const char*>(data)));
		unbind();
	}

	m_size = new_size;
}

void VertexBuffer::resize(size_t newSize) {
	size_t new_capacity = std::max(m_capacity * 2, newSize);
	void* new_block = operator new(new_capacity);

	memcpy(new_block, m_data, m_size);
	operator delete(m_data);

	m_data = new_block;
	m_capacity = new_capacity;
}

void VertexBuffer::set_data_open_gl(bool initialUpload) {
	bind();

	if (initialUpload) {
		GLCall(glBufferData(GL_ARRAY_BUFFER, m_capacity, nullptr, GL_DYNAMIC_DRAW));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, m_size, m_data));
	} else {
		GLCall(glBufferSubData(GL_ARRAY_BUFFER,
		    m_size_before_last_append,
		    m_size - m_size_before_last_append,
		    static_cast<char*>(m_data) + m_size_before_last_append));
	}

	unbind();
}

}
