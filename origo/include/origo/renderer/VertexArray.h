#pragma once

#include "origo/renderer/VertexBuffer.h"

namespace Origo {
class VertexArray {
public:
	VertexArray();

	void Bind() const;
	void Unbind() const;
	void ConnectBuffer(const VertexBuffer& buffer) const;

private:
	GLuint m_BufferId {};
};
}
