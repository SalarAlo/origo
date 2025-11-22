#include "origo/assets/Mesh.h"
#include "origo/assets/MeshSource.h"
#include "origo/assets/PrimitiveShape.h"

namespace Origo {
Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    : m_Source(MakeScope<MeshSourceRaw>(vertices, indices))
    , m_VertexArray()
    , m_VertexBuffer(vertices)
    , m_IndexBuffer(indices) {
	Init();
}

Mesh::Mesh(PrimitiveShape shape)
    : m_Source(MakeScope<MeshSourcePrimitiveShape>(shape))
    , m_VertexArray()
    , m_IndexBuffer(m_Source->GetData().Indices)
    , m_VertexBuffer(m_Source->GetData().Vertices) {
	Init();
}

Mesh::Mesh(std::string_view meshPath, size_t idx, const MeshData& data)
    : m_Source(MakeScope<MeshSourceExternal>(meshPath, idx, data))
    , m_VertexArray()
    , m_IndexBuffer(m_Source->GetData().Indices)
    , m_VertexBuffer(m_Source->GetData().Vertices) {
	Init();
}

void Mesh::Init() {
	m_VertexArray.Bind();

	m_VertexBuffer.Bind();
	m_IndexBuffer.Bind();

	m_VertexArray.ConnectBuffer(m_VertexBuffer);
	m_IndexBuffer.Unbind();
}

void Mesh::Render() const {
	m_VertexArray.ConnectBuffer(m_VertexBuffer);
	m_VertexArray.Bind();

	GLCall(glDrawElements(GL_TRIANGLES, m_IndexBuffer.GetElementCount(), GL_UNSIGNED_INT, nullptr));
}

}
