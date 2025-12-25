#include "origo/scene/CameraComponent.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawableRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::Register<Origo::CameraComponent>("Camera", "./icons/Camera.svg", [](Origo::CameraComponent& cam) {
		float fov { cam.GetCamera().GetFOV() };
		bool isPrimary { cam.IsPrimary };

		ComponentUI::DrawFloatControl("FOV", fov);
		ComponentUI::DrawBoolControl("Is Primary", isPrimary);

		cam.IsPrimary = isPrimary;
		cam.GetCamera().SetFOV(fov);
	});

	return true;
}();

}
