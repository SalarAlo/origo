#pragma once

#include "origo/assets/AssetManager.h"
#include "origo/scene/Component.h"

namespace Origo {

class PointLight : public Component {
public:
	std::string GetComponentName() const override { return "Point Light"; }

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

	void Serialize(ISerializer& s) const override {
		s.Write("intensity", m_Intensity);

		s.Write("color_r", m_LightColor.r);
		s.Write("color_g", m_LightColor.g);
		s.Write("color_b", m_LightColor.b);

		s.Write("atten_constant", m_Constant);
		s.Write("atten_linear", m_Linear);
		s.Write("atten_quadratic", m_Quadratic);

		if (m_ShaderTarget.has_value()) {
			auto uuid = AssetManager::GetInstance().GetUUID(*m_ShaderTarget);
			if (uuid.has_value())
				s.Write("shader_target", uuid->ToString());
			else
				s.Write("shader_target", "0");
		} else {
			s.Write("shader_target", "0");
		}
	}

	void Deserialize(ISerializer& s) override {
		s.TryRead("intensity", m_Intensity);

		s.TryRead("color_r", m_LightColor.r);
		s.TryRead("color_g", m_LightColor.g);
		s.TryRead("color_b", m_LightColor.b);

		s.TryRead("atten_constant", m_Constant);
		s.TryRead("atten_linear", m_Linear);
		s.TryRead("atten_quadratic", m_Quadratic);

		std::string shaderTarget;
		if (s.TryRead("shader_target", shaderTarget) && shaderTarget != "0") {
			m_ShaderTarget = AssetManager::GetInstance().GetHandleByUUID(UUID::FromString(shaderTarget));
		} else {
			m_ShaderTarget.reset();
		}
	}

private:
	float m_Intensity { 1.0f };
	glm::vec3 m_LightColor { 1.0f };

	float m_Constant { 1.0f };
	float m_Linear { 0.09f };
	float m_Quadratic { 0.032f };

	OptionalAssetHandle m_ShaderTarget {};
};

}
