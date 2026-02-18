#pragma once

#include "Component.h"

#include "origo/assets/AssetManager.h"

namespace Origo {

class PointLightComponent : public Component {
public:
	std::string GetComponentName() const override { return "Point Light"; }

	void SetIntensity(float intensity) { m_Intensity = intensity; }
	float GetIntensity() const { return m_Intensity; }

	void SetLightColor(const Vec3& color) { m_LightColor = color; }
	Vec3 GetLightColor() const { return m_LightColor; }

	void SetAttenuation(float c, float l, float q) {
		m_Constant = c;
		m_Linear = l;
		m_Quadratic = q;
	}

	float GetConstant() const { return m_Constant; }
	float GetLinear() const { return m_Linear; }
	float GetQuadratic() const { return m_Quadratic; }

private:
	float m_Intensity { 1.0f };
	Vec3 m_LightColor { 1.0f };

	float m_Constant { 1.0f };
	float m_Linear { 0.09f };
	float m_Quadratic { 0.032f };
};

}
