#pragma once

#include "origo/assets/MeshData.h"

namespace Origo {
enum class PrimitiveShape {
	Cube,
	Sphere,
	Triangle,
	Quad,
	Cone
};

MeshData GetDataFromShape(PrimitiveShape shape);

};
