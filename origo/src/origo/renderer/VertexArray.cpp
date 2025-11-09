#include "origo/renderer/VertexArray.h"
#include "origo/renderer/VertexLayout.h"

#include "origo/renderer/Helpers.h"

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

void VertexArray::ConnectBuffer(const VertexBuffer& buffer) const {
	Bind();
	buffer.Bind();

	const auto& attribs { STANDARD_LAYOUT.GetAttributes() };
	int stride {};
	for (int i {}; i < attribs.size(); i++) {
		const auto& attrib { attribs[i] };
		const auto size { Glsizeof(attrib.Type) * attrib.Amount };
		stride += size;
	}

	long offset {};
	for (int i {}; i < attribs.size(); i++) {
		const auto& attrib { attribs[i] };
		const auto size { Glsizeof(attrib.Type) * attrib.Amount };
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, attrib.Amount, attrib.Type, attrib.Normalized, stride, reinterpret_cast<void*>(offset)));
		offset += size;
	}

	buffer.Unbind();
	Unbind();
}

}
