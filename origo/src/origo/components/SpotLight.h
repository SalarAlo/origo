#pragma once

#include "Component.h"

namespace Origo {

class SpotLight : public Component {
public:
	float Intensity { 1.0f };
	Vec3 LightColor { 1.0f };

	float InnerCutoff { 0.9f };
	float OuterCutoff { 0.85f };

	float Constant { 1.0f };
	float Linear { 0.09f };
	float Quadratic { 0.032f };
};

}
