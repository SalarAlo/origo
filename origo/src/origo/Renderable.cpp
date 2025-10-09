#include "origo/Renderable.h"

namespace Origo {
Renderable::Renderable(const Mesh& mesh)
    : m_Mesh(mesh)
    , m_VertexArray()
    , m_VertexBuffer(m_Mesh.Vertices)
    , m_IndexBuffer(m_Mesh.Indices) {
	m_VertexArray.Bind();
	m_VertexBuffer.Bind();
	m_IndexBuffer.Bind();

	m_VertexArray.ConnectBufferWithLayout(m_Mesh.Layout, m_VertexBuffer);

	m_VertexArray.Unbind();
	m_VertexBuffer.Unbind();
	m_IndexBuffer.Unbind();
}

void Renderable::Render(const Shader& shader) {
	m_VertexArray.Bind();
	m_IndexBuffer.Bind();

	shader.UseProgram();
	glDrawElements(GL_TRIANGLES, m_IndexBuffer.GetElementCount(), GL_UNSIGNED_INT, nullptr);
}
}
