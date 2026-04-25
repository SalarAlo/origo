#pragma once

#include "origo/assets/AssetManager.h"

namespace Origo {

class PrimitiveShapeCache : public Singleton<PrimitiveShapeCache> {
public:
	void create_all_primitive_shapes();

	AssetHandle get_cube_mesh();
	AssetHandle get_wire_cube_mesh();
	AssetHandle get_wire_cone_mesh();
	AssetHandle get_sphere_mesh();
	AssetHandle get_quad_mesh();
	AssetHandle get_cone_mesh();
};

}
