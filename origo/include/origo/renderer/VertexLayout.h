#pragma once

#include "VertexAttribute.h"

namespace Origo {

class VertexLayout {
public:
	template <typename>
	void AddAttribute(unsigned int amount, bool normalized = false, VertexAttributeSemantic semantic = VertexAttributeSemantic::None);
	std::vector<VertexAttribute>& GetAttributes();

private:
	std::vector<VertexAttribute> m_Attributes;
};

template <>
void VertexLayout::AddAttribute<unsigned int>(unsigned int amount, bool normalized, VertexAttributeSemantic semantic);
template <>
void VertexLayout::AddAttribute<float>(unsigned int amount, bool normalized, VertexAttributeSemantic semantic);

}
