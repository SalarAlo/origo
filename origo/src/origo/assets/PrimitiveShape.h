#pragma once

#include "origo/assets/MeshData.h"

namespace Origo {
enum class PrimitiveShape {
	Cube,
	WireCube,
	WireCone,
	Sphere,
	Triangle,
	Quad,
	Cone
};

MeshData get_data_from_shape(PrimitiveShape shape);

};
