#pragma once

#include "origo/assets/MeshData.h"

namespace Origo {
enum class PrimitiveShape {
	Cube,
	Sphere,
	Triangle,
	Quad,
};

MeshData GetDataFromShape(PrimitiveShape shape);

};
