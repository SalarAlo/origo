#pragma once

#include "Mesh.h"

class ModelLoader {
public:
	static std::vector<Mesh> LoadModel(const std::string& path, const VertexLayout& layout);
};
