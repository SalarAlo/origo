#pragma once

#include "origo/components/Component.h"

namespace Origo {
class TerrainComponent : public Component {
public:
	float Frequency { 0.5f };
	float Amplitude { 2.0f };
	int Octaves { 3 };
	float Scale { 0.02f };
	int Size { 10 };

	std::string get_component_name() const { return "Terrain"; };
};

}
