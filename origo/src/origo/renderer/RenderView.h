
#pragma once

namespace Origo {

struct RenderView {
	glm::mat4 Projection { 1.0f };
	glm::mat4 View { 1.0f };
	Vec3 CameraForward { 0, 0, 1 };
	Vec3 CameraPosition { 0, 0, 0 };
};
}
