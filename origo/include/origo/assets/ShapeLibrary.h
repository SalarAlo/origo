#pragma once

#include "CubeVertices.h"
#include "origo/renderer/Mesh.h"

namespace Origo::ShapeLibrary {
enum PrimitiveShape {
	CUBE,
};

Ref<Mesh> Load(PrimitiveShape shape);

}
