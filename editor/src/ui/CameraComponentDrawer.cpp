#include <glm/vec3.hpp>

#include "origo/components/CameraComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::CameraComponent>("Camera", "./icons/Camera.svg", [](Origo::CameraComponent& cam) {
		ComponentUI::draw_float_control("FOV", cam.CameraObj.FOV);
		ComponentUI::draw_bool_control("Is Primary", cam.IsPrimary);
	});

	return true;
}();

}
