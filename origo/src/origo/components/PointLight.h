#pragma once

#include "Component.h"

namespace Origo {

struct PointLightComponent : Component {
	std::string get_component_name() const override { return "Point Light"; }

	void set_attenuation(float c, float l, float q) {
		Constant = c;
		Linear = l;
		Quadratic = q;
	}

	float Intensity { 1.0f };
	Vec3 Color { 1.0f };

	float Constant { 1.0f };
	float Linear { 0.09f };
	float Quadratic { 0.032f };
};

}
