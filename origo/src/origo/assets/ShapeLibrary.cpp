#include "origo/assets/ShapeLibrary.h"
#include "origo/assets/CubeVertices.h"

namespace Origo::ShapeLibrary {

Ref<Mesh> Load(PrimitiveShape shape) {
	static Ref<Mesh> CUBE_SHAPE { MakeRef<Mesh>(CUBE_VERTICES, CUBE_INDICES) };

	switch (shape) {
	case PrimitiveShape::CUBE:
		return CUBE_SHAPE;
		break;
	default:
		return nullptr;
	}
}

}
