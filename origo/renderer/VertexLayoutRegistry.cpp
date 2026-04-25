#include "origo/renderer/VertexLayoutRegistry.h"

#include "origo/renderer/VertexLayout.h"

namespace Origo::VertexLayoutRegistry {
static std::vector<VertexLayout> s_layouts {};

int register_layout(VertexLayout& layout) {
	int id = s_layouts.size();
	s_layouts.push_back(layout);

	return id;
}

VertexLayout* get(int id) {
	return &s_layouts[id];
}

}
