#pragma once

namespace Origo {

struct MeshData {
	MeshData() = default;
	MeshData(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
	    : Vertices(vertices)
	    , Indices(indices) {
	}

	std::vector<float> Vertices {};
	std::vector<unsigned int> Indices {};
};

}
