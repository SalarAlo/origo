#include <glm/vec3.hpp>

#include "origo/components/CameraComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::CameraComponent>("Camera", "./icons/Camera.svg", [](Origo::CameraComponent& cam) {
		ComponentUI::DrawFloatControl("FOV", cam.CameraObj.FOV);
		ComponentUI::DrawBoolControl("Is Primary", cam.IsPrimary);
	});

	return true;
}();

}
