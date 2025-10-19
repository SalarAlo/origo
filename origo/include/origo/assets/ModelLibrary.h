#pragma once

#include "origo/renderer/Mesh.h"

namespace Origo {

namespace ModelLibrary {
	std::vector<Ref<Mesh>> Create(const std::string& path);
};

}
