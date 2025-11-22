#include "origo/renderer/VertexLayout.h"
#include "origo/renderer/VertexAttribute.h"

namespace Origo {

template <>
void VertexLayout::AddAttribute<unsigned int>(unsigned int amount, bool normalized, VertexAttributeSemantic semantic) {
	m_Attributes.push_back({ .Semantic = semantic,
	    .Type = GL_UNSIGNED_INT,
	    .Amount = amount,
	    .Normalized = normalized ? GL_TRUE : GL_FALSE });
}

template <>
void VertexLayout::AddAttribute<float>(unsigned int amount, bool normalized, VertexAttributeSemantic semantic) {
	m_Attributes.push_back({ .Semantic = semantic,
	    .Type = GL_FLOAT,
	    .Amount = amount,
	    .Normalized = normalized ? GL_TRUE : GL_FALSE });
}

std::vector<VertexAttribute>& VertexLayout::GetAttributes() {
	return m_Attributes;
}
}
