#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace Origo {

class DirectionalLight : public Component {
public:
	DirectionalLight() = default;
	DirectionalLight(int shinyFactor, glm::vec3 lightColor, float ambientFactor)
	    : m_Intensity(shinyFactor)
	    , m_LightColor(lightColor)
	    , m_AmbientFactor(ambientFactor) { }

	std::string GetComponentName() const override { return "Directional Light"; };

	void SetShaderTarget(const AssetHandle& handle) { m_ShaderTarget = handle; }
	OptionalAssetHandle GetShaderTarget() { return m_ShaderTarget; }

	void SetIntensity(float factor) { m_Intensity = factor; }
	float GetIntensity() const { return m_Intensity; }

	void SetLightColor(const glm::vec3& color) { m_LightColor = color; }
	glm::vec3 GetLightColor() const { return m_LightColor; }

	void SetAmbientFactor(float ambientFactor) { m_AmbientFactor = ambientFactor; }
	float GetAmbientFactor() const { return m_AmbientFactor; }

	void Serialize(ISerializer& backend) const override;
	void Deserialize(ISerializer& backend) override;

private:
	float m_Intensity { 1.0f };
	glm::vec3 m_LightColor { 1.0f };
	float m_AmbientFactor { 0.2f };

	OptionalAssetHandle m_ShaderTarget {};
};

}
