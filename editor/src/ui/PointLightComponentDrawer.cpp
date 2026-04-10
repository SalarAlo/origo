#include <glm/vec3.hpp>

#include "origo/components/PointLightComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::PointLightComponent>(
	    "PointLight",
	    "icons/Light.svg",
	    [](Origo::PointLightComponent& light) {
		    ComponentUI::draw_float_control("Intensity", light.Intensity);
		    ComponentUI::draw_color_control("Color", light.Color);

		    ComponentUI::draw_float_control("Attenuation (Constant)", light.Constant);
		    ComponentUI::draw_float_control("Attenuation (Linear)", light.Linear);
		    ComponentUI::draw_float_control("Attenuation (Quadratic)", light.Quadratic);
	    });

	return true;
}();

}
