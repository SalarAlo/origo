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

	VertexLayout layout;

	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Position);
	layout.AddAttribute<float>(3, false, VertexAttributeSemantic::Normal);
	layout.AddAttribute<float>(2, false, VertexAttributeSemantic::TexCoord);

	const auto& attribs { layout.GetAttributes() };

	for (int i {}; i < attribs.size(); i++) {
		const auto& attrib { attribs[i] };
		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, attrib.Amount, attrib.Type, attrib.Normalized, layout.GetStride(), attrib.Offset));
	}

	buffer.Unbind();
	Unbind();
}

}
