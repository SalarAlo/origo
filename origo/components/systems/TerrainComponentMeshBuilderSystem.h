#pragma once

#include "origo/assets/MeshData.h"

#include "origo/scene/IUpdateSystem.h"

namespace Origo {
class TerrainComponentMeshBuilderSystem : public IUpdateSystem {
public:
	void update(Scene* scene, float dt) override;
	static MeshData construct_mesh_data_from_heightmap(
	    const std::vector<float>& height_map,
	    int size_x,
	    int size_z,
	    float cell_size);
};

}
