#pragma once

#include "origo/assets/MeshData.h"

namespace Origo {
enum class PrimitiveShape {
	Cube,
	Sphere,
	Triangle
};

MeshData GetDataFromShape(PrimitiveShape shape);

};
