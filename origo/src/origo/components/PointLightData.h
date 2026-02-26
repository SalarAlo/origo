#pragma once

namespace Origo {

struct PointLightData {
	Vec3 Position { 0.0f };
	float Intensity { 1.0f };

	Vec3 Color { 1.0f };
	float Constant { 1.0f };

	float Linear { 0.09f };
	float Quadratic { 0.032f };
};

}
