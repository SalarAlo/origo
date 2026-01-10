#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace Origo {

class PointLight : public Component {
public:
	std::string GetName() const override { return "Point Light"; }

	void SetShaderTarget(const AssetHandle& handle) { m_ShaderTarget = handle; }
	OptionalAssetHandle GetShaderTarget() const { return m_ShaderTarget; }

	void SetIntensity(float intensity) { m_Intensity = intensity; }
	float GetIntensity() const { return m_Intensity; }

	void SetLightColor(const glm::vec3& color) { m_LightColor = color; }
	glm::vec3 GetLightColor() const { return m_LightColor; }

	void SetAttenuation(float c, float l, float q) {
		m_Constant = c;
		m_Linear = l;
		m_Quadratic = q;
	}

	float Constant() const { return m_Constant; }
	float Linear() const { return m_Linear; }
	float Quadratic() const { return m_Quadratic; }

private:
	float m_Intensity { 1.0f };
	glm::vec3 m_LightColor { 1.0f };

	float m_Constant { 1.0f };
	float m_Linear { 0.09f };
	float m_Quadratic { 0.032f };

	OptionalAssetHandle m_ShaderTarget {};
};

}
