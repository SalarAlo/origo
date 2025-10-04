#pragma once

#include "engine/VertexBuffer.h"
#include "engine/VertexLayout.h"

class VertexArray {
public:
	VertexArray();

	void Bind() const;
	void Unbind() const;
	void ConnectBufferWithLayout(const VertexLayout& layout, const VertexBuffer& buffer);

private:
	GLuint m_BufferId {};
};
