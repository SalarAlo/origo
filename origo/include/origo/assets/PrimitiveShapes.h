#pragma once

#include "CubeVertices.h"
#include "origo/renderer/Mesh.h"

namespace Origo::Assets {
enum PrimitiveShape {
	CUBE,
};

Ref<Mesh> LoadShape(PrimitiveShape shape);

}
