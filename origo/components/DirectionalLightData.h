#pragma once

namespace Origo {

struct DirectionalLightData {
	Vec3 Direction { 0.0f, -1.0f, 0.0f };
	float Intensity { 1.0f };

	Vec3 Color { 1.0f };
	float Ambient { 0.2f };
};

}
