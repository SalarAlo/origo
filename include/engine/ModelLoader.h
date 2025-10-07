#pragma once

#include "engine/Mesh.h"

class ModelLoader {
public:
	static std::vector<Mesh> LoadModel(const std::string& path, const VertexLayout& layout);
};
