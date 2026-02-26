#include "origo/renderer/VertexLayout.h"

#include "origo/renderer/Helpers.h"
#include "origo/renderer/VertexAttribute.h"
#include "origo/renderer/VertexLayoutRegistry.h"

namespace Origo {

template <>
void VertexLayout::add_attribute<unsigned int>(unsigned int amount, bool normalized, VertexAttributeSemantic semantic) {
	m_attributes.push_back({ .Semantic = semantic,
	    .Type = GL_UNSIGNED_INT,
	    .Amount = amount,
	    .Normalized = normalized ? GL_TRUE : GL_FALSE,
	    .Offset = reinterpret_cast<void*>(m_stride) });
	m_stride += amount * glsizeof(GL_UNSIGNED_INT);
}

template <>
void VertexLayout::add_attribute<float>(unsigned int amount, bool normalized, VertexAttributeSemantic semantic) {
	m_attributes.push_back({ .Semantic = semantic,
	    .Type = GL_FLOAT,
	    .Amount = amount,
	    .Normalized = normalized ? GL_TRUE : GL_FALSE,
	    .Offset = reinterpret_cast<void*>(m_stride) });
	m_stride += amount * glsizeof(GL_FLOAT);
}

std::vector<VertexAttribute>& VertexLayout::get_attributes() {
	return m_attributes;
}

int VertexLayout::get_static_mesh_layout() {
	static int id = [] {
		VertexLayout layout {};
		layout.add_attribute<float>(3, false, VertexAttributeSemantic::Position);
		layout.add_attribute<float>(3, false, VertexAttributeSemantic::Normal);
		layout.add_attribute<float>(2, false, VertexAttributeSemantic::TexCoord);
		return VertexLayoutRegistry::register_layout(layout);
	}();
	return id;
}

}
