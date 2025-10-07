#include "engine/VertexArray.h"

#include "engine/Helpers.h"

VertexArray::VertexArray() {
	glGenVertexArrays(1, &m_BufferId);
}

void VertexArray::Bind() const {
	glBindVertexArray(m_BufferId);
}

void VertexArray::Unbind() const {
	glBindVertexArray(0);
}

void VertexArray::ConnectBufferWithLayout(const VertexLayout& layout, const VertexBuffer& buffer) {
	Bind();
	buffer.Bind();

	const auto& attribs { layout.GetAttributes() };
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
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, attrib.Amount, attrib.Type, attrib.Normalized, stride, reinterpret_cast<void*>(offset));
		offset += size;
	}
}
