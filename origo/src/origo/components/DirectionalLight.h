#pragma once

#include "Component.h"

namespace Origo {

class DirectionalLightComponent : public Component {
public:
	DirectionalLightComponent() = default;
	DirectionalLightComponent(int shinyFactor, Vec3 lightColor, float ambientFactor)
	    : m_Intensity(shinyFactor)
	    , m_LightColor(lightColor)
	    , m_AmbientFactor(ambientFactor) { }

	std::string GetComponentName() const override { return "Directional Light"; };

	void SetIntensity(float factor) { m_Intensity = factor; }
	float GetIntensity() const { return m_Intensity; }

	void SetLightColor(const Vec3& color) { m_LightColor = color; }
	Vec3 GetLightColor() const { return m_LightColor; }

	void SetAmbientFactor(float ambientFactor) { m_AmbientFactor = ambientFactor; }
	float GetAmbientFactor() const { return m_AmbientFactor; }

private:
	float m_Intensity { 1.0f };
	Vec3 m_LightColor { 1.0f };
	float m_AmbientFactor { 0.2f };
};

}
