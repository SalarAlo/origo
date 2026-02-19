#pragma once

#include "Component.h"

#include "origo/assets/AssetManager.h"

namespace Origo {

class SpotLight : public Component {
public:
	std::string GetComponentName() const override { return "Spot Light"; }

	void SetCutoffs(float inner, float outer) {
		InnerCutoff = inner;
		OuterCutoff = outer;
	}

	void SetAttenuation(float c, float l, float q) {
		Constant = c;
		Linear = l;
		Quadratic = q;
	}

	float Intensity { 1.0f };
	Vec3 LightColor { 1.0f };

	float InnerCutoff { 0.9f };
	float OuterCutoff { 0.85f };

	float Constant { 1.0f };
	float Linear { 0.09f };
	float Quadratic { 0.032f };

	OptionalAssetHandle ShaderTarget {};
};

}
