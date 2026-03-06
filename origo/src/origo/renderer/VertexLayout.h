#pragma once

#include "VertexAttribute.h"

namespace Origo {

class VertexLayout {
public:
	template <typename>
	void add_attribute(unsigned int amount, bool normalized = false, VertexAttributeSemantic semantic = VertexAttributeSemantic::None);

	std::vector<VertexAttribute>& get_attributes();
	size_t get_stride() { return m_stride; }
	static int get_static_mesh_layout();
	static int get_static_mesh_tangent_layout();

private:
	std::vector<VertexAttribute> m_attributes {};
	size_t m_stride {};
};

template <>
void VertexLayout::add_attribute<unsigned int>(unsigned int amount, bool normalized, VertexAttributeSemantic semantic);

template <>
void VertexLayout::add_attribute<float>(unsigned int amount, bool normalized, VertexAttributeSemantic semantic);

}
