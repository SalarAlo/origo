#pragma once

#include "VertexAttribute.h"

namespace Origo {

class VertexLayout {
public:
	template <typename>
	void AddAttribute(unsigned int amount, bool normalized = false);
	const std::vector<VertexAttribute>& GetAttributes() const;

private:
	std::vector<VertexAttribute> m_Attributes;
};

template <>
void VertexLayout::AddAttribute<unsigned int>(unsigned int amount, bool normalized);
template <>
void VertexLayout::AddAttribute<float>(unsigned int amount, bool normalized);

inline const VertexLayout STANDARD_LAYOUT = [] {
	VertexLayout layout;
	layout.AddAttribute<float>(3); // position
	layout.AddAttribute<float>(3); // normal
	layout.AddAttribute<float>(2); // uv
	return layout;
}();

}
