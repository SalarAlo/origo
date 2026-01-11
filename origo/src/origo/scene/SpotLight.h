#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace Origo {

class SpotLight : public Component {
public:
	std::string GetComponentName() const override { return "Spot Light"; }

	void SetShaderTarget(const AssetHandle& handle) { m_ShaderTarget = handle; }
	OptionalAssetHandle GetShaderTarget() const { return m_ShaderTarget; }

	void SetIntensity(float intensity) { m_Intensity = intensity; }
	float GetIntensity() const { return m_Intensity; }

	void SetLightColor(const glm::vec3& color) { m_LightColor = color; }
	glm::vec3 GetLightColor() const { return m_LightColor; }

	void SetCutoffs(float inner, float outer) {
		m_InnerCutoff = inner;
		m_OuterCutoff = outer;
	}

	float InnerCutoff() const { return m_InnerCutoff; }
	float OuterCutoff() const { return m_OuterCutoff; }

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

	float m_InnerCutoff { 0.9f };
	float m_OuterCutoff { 0.85f };

	float m_Constant { 1.0f };
	float m_Linear { 0.09f };
	float m_Quadratic { 0.032f };

	OptionalAssetHandle m_ShaderTarget {};
};

}
