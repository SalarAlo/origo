#include "origo/renderer/VertexArray.h"

namespace Origo {

VertexArray::VertexArray() {
	GLCall(glGenVertexArrays(1, &m_buffer_id));
}

VertexArray::~VertexArray() {
	GLCall(glDeleteVertexArrays(1, &m_buffer_id));
}

void VertexArray::bind() const {
	GLCall(glBindVertexArray(m_buffer_id));
}

void VertexArray::unbind() const {
	GLCall(glBindVertexArray(0));
}

}
