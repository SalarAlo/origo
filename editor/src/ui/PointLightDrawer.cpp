#include <glm/vec3.hpp>

#include "origo/components/PointLight.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::PointLightComponent>(
	    "PointLight",
	    "icons/Light.svg",
	    [](Origo::PointLightComponent& light) {
		    ComponentUI::DrawFloatControl("Intensity", light.Intensity);
		    ComponentUI::DrawColorControl("Color", light.Color);

		    ComponentUI::DrawFloatControl("Attenuation (Constant)", light.Constant);
		    ComponentUI::DrawFloatControl("Attenuation (Linear)", light.Linear);
		    ComponentUI::DrawFloatControl("Attenuation (Quadratic)", light.Quadratic);
	    });

	return true;
}();

}
