#include "origo/renderer/Batch.h"

namespace Origo {
Batch::Batch() {
	m_VertexArray.ConnectBuffer(m_VertexBuffer);
}

void Batch::AddMesh(Ref<Mesh> mesh) {
	m_VertexBuffer.AddData(mesh->Vertices);
	m_IndexBuffer.AddData(mesh->Indices);
}

void Batch::Render() const {
	m_VertexArray.Bind();
	m_VertexBuffer.Bind();
	m_IndexBuffer.Bind();

	glDrawElements(GL_TRIANGLES, m_IndexBuffer.GetElementCount(), GL_UNSIGNED_INT, nullptr);
}
}
