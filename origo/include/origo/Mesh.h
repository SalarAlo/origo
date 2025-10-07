#pragma once

#include "VertexLayout.h"

struct Mesh {
	std::vector<float> Vertices {};
	std::vector<unsigned int> Indices {};
	VertexLayout Layout {};
};
