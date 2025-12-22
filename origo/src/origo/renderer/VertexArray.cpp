#include "origo/renderer/VertexArray.h"

namespace Origo {

VertexArray::VertexArray() {
	GLCall(glGenVertexArrays(1, &m_BufferId));
}

VertexArray::~VertexArray() {
	GLCall(glDeleteVertexArrays(1, &m_BufferId));
}

void VertexArray::Bind() const {
	GLCall(glBindVertexArray(m_BufferId));
}

void VertexArray::Unbind() const {
	GLCall(glBindVertexArray(0));
}

}
