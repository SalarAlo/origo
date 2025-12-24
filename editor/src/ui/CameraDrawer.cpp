#include "origo/scene/CameraComponent.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawableRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::Register<Origo::CameraComponent>("Camera", "./icons/Camera.svg", [](Origo::CameraComponent& cam) {
		float fov { cam.GetCamera().GetFOV() };

		ComponentUI::DrawFloatControl("FOV", fov);

		cam.GetCamera().SetFOV(fov);
	});

	return true;
}();

}
