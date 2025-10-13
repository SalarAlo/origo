#include "origo/renderer/Mesh.h"

namespace Origo {
Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    : m_VertexBuffer(vertices)
    , m_IndexBuffer(indices)
    , m_VertexArray() {
	m_VertexArray.Bind();
	m_IndexBuffer.Bind();
	m_VertexArray.ConnectBuffer(m_VertexBuffer);
}

void Mesh::Render() const {
	m_VertexArray.Bind();
	glDrawElements(GL_TRIANGLES, m_IndexBuffer.GetElementCount(), GL_UNSIGNED_INT, nullptr);
}
}
