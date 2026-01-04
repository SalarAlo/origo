#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace Origo {

class Light : public Component {
public:
	Light() = default;
	Light(int shinyFactor, glm::vec3 lightColor, float ambientFactor)
	    : m_ShinyFactor(shinyFactor)
	    , m_LightColor(lightColor)
	    , m_AmbientFactor(ambientFactor) { }

	std::string GetName() const override { return "Light"; };

	void SetShaderTarget(const AssetHandle& handle) { m_ShaderTarget = handle; }
	const AssetHandle& GetShaderTarget() { return m_ShaderTarget; }

	void SetShinyFactor(int factor) { m_ShinyFactor = factor; }
	int GetShinyFactor() const { return m_ShinyFactor; }

	void SetLightColor(const glm::vec3& color) { m_LightColor = color; }
	glm::vec3 GetLightColor() const { return m_LightColor; }

	void SetAmbientFactor(float ambientFactor) { m_AmbientFactor = ambientFactor; }
	float GetAmbientFactor() const { return m_AmbientFactor; }

private:
	int m_ShinyFactor { 32 };
	glm::vec3 m_LightColor { 1.0f };
	float m_AmbientFactor { 0.2f };

	AssetHandle m_ShaderTarget {};
};

}
