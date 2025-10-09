#pragma once

#include "VertexLayout.h"

namespace Origo {
struct Mesh {
	std::vector<float> Vertices {};
	std::vector<unsigned int> Indices {};
	VertexLayout Layout {};
};
}
