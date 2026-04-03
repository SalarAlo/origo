
#include "origo/components/CameraShakeComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::CameraShakeComponent>("Camera Shake", "icons/Camera.svg", [](Origo::CameraShakeComponent& camera_shake) {
		ComponentUI::draw_float_control("Duration", camera_shake.Duration);
		ComponentUI::draw_float_control("Delay", camera_shake.Delay);

		ComponentUI::draw_float_control("Amplitude", camera_shake.Amplitude);
		ComponentUI::draw_float_control("Frequency", camera_shake.Frequency);
	});

	return true;
}();

}
