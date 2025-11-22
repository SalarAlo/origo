#pragma once

#include "origo/renderer/VertexBuffer.h"

namespace Origo {
class VertexArray {
public:
	VertexArray();
	~VertexArray();

	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;

	VertexArray(VertexArray&&) = delete;
	VertexArray& operator=(VertexArray&&) = delete;

	void Bind() const;
	void Unbind() const;

	void ConnectBuffer(const VertexBuffer& buffer) const;

private:
	GLuint m_BufferId {};
};
}
