#include "origo/renderer/VertexLayoutRegistry.h"
#include "origo/renderer/VertexLayout.h"

namespace Origo::VertexLayoutRegistry {
static std::vector<VertexLayout> s_Layouts {};

int Register(VertexLayout& layout) {
	int id = s_Layouts.size();
	s_Layouts.push_back(layout);

	return id;
}

VertexLayout* Get(int id) {
	return &s_Layouts[id];
}

}
