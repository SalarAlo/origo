#pragma once

namespace Origo {

struct PointLightData {
	glm::vec3 position { 0.0f };
	float intensity { 1.0f };

	glm::vec3 color { 1.0f };
	float constant { 1.0f };

	float linear { 0.09f };
	float quadratic { 0.032f };
};

}
