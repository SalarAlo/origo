#pragma once

#include "Component.h"

namespace Origo {

class DirectionalLightComponent : public Component {
public:
	DirectionalLightComponent() = default;
	DirectionalLightComponent(int intensity, Vec3 lightColor, float ambientFactor)
	    : Intensity(intensity)
	    , LightColor(lightColor)
	    , AmbientFactor(ambientFactor) { }

	std::string GetComponentName() const override { return "Directional Light"; };

	float Intensity { 1.0f };
	Vec3 LightColor { 1.0f };
	float AmbientFactor { 0.2f };
};

}
