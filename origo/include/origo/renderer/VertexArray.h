#pragma once

#include "origo/renderer/VertexBuffer.h"
#include "origo/renderer/VertexLayout.h"

namespace Origo {
class VertexArray {
public:
	VertexArray();

	void Bind() const;
	void Unbind() const;
	void ConnectBufferWithLayout(const VertexLayout& layout, const VertexBuffer& buffer);

private:
	GLuint m_BufferId {};
};
}
