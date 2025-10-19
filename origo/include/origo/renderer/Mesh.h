#pragma once

#include "origo/core/Identifiable.h"
#include "origo/renderer/IndexBuffer.h"
#include "origo/renderer/VertexArray.h"
#include "origo/renderer/VertexBuffer.h"

namespace Origo {
class Mesh : public Identifiable {
public:
	Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
	void Render() const;

private:
	VertexArray m_VertexArray;
	VertexBuffer m_VertexBuffer;
	IndexBuffer m_IndexBuffer;
};
}
