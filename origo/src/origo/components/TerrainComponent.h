#pragma once

#include "origo/assets/AssetManager.h"

#include "origo/components/Component.h"

#include "origo/core/Random.h"

namespace Origo {
class TerrainComponent : public Component {
public:
	Random::PerlinSettings Perlin {};
	int Size { 10 };
	float Detail { 0.5f };
	float Height { 4.0f };

	OptionalAssetHandle TerrainMesh {};
	bool RebuildNecessary { true };

	std::string get_component_name() const { return "Terrain"; };
};

}
