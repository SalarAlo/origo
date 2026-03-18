#pragma once

#include "origo/assets/AssetManager.h"

#include "origo/components/TerrainComponent.h"

namespace Origo {
class TerrainGenerator {
public:
	TerrainGenerator(TerrainComponent& terrain_component)
	    : m_terrain_component(terrain_component) {
	    };

	OptionalAssetHandle get_mesh();

private:
	TerrainComponent& m_terrain_component;
};

}
