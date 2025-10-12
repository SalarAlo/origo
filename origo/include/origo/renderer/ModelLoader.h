#pragma once

#include "origo/renderer/Mesh.h"

namespace Origo {

class ModelLoader {
public:
	static std::vector<Mesh> LoadModel(const std::string& path, const VertexLayout& layout);
};

}
