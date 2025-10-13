#pragma once

#include "origo/renderer/IndexBuffer.h"
#include "origo/renderer/Mesh.h"
#include "origo/renderer/VertexArray.h"
#include "origo/renderer/VertexBuffer.h"

namespace Origo {
class Batch {
public:
	Batch();
	void AddMesh(Ref<Mesh> mesh);
	void Render() const;

private:
	IndexBuffer m_IndexBuffer { {} };
	VertexBuffer m_VertexBuffer { {} };
	VertexArray m_VertexArray {};
};

}
