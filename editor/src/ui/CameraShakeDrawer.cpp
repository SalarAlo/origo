#include <glm/vec3.hpp>

#include "origo/components/CameraShakeComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::CameraShakeComponent>("Camera Shake", "./icons/Camera.svg", [](Origo::CameraShakeComponent& camera_shake) {
		ComponentUI::draw_float_control("Shake Duration", camera_shake.ShakeDuration);
		ComponentUI::draw_float_control("Shake Delay", camera_shake.ShakeDelay);
		ComponentUI::draw_float_control("Shake Amplitude", camera_shake.ShakeAmplitude);
		ComponentUI::draw_float_control("Shake Frequency", camera_shake.ShakeFrequency);
	});

	return true;
}();

}
