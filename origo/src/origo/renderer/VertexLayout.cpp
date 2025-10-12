#include "origo/renderer/VertexLayout.h"

namespace Origo {
template <>
void VertexLayout::AddAttribute<float>(unsigned int amount, bool normalized) {
	m_Attributes.push_back({ .Type = GL_FLOAT,
	    .Amount = amount,
	    .Normalized = normalized ? GL_TRUE : GL_FALSE });
}

template <>
void VertexLayout::AddAttribute<unsigned int>(unsigned int amount, bool normalized) {
	m_Attributes.push_back({ .Type = GL_UNSIGNED_INT,
	    .Amount = amount,
	    .Normalized = normalized ? GL_TRUE : GL_FALSE });
}

const std::vector<VertexAttribute>& VertexLayout::GetAttributes() const {
	return m_Attributes;
}
}
