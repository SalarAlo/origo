#include "origo/assets/Asset.h"
#include "origo/components/PointLight.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::PointLightComponent>(
	    "PointLight",
	    "icons/Light.svg",
	    [](Origo::PointLightComponent& light) {
		    float intensity { light.GetIntensity() };
		    auto color { light.GetLightColor() };

		    float constant { light.GetConstant() };
		    float linear { light.GetLinear() };
		    float quadratic { light.GetQuadratic() };

		    ComponentUI::DrawFloatControl("Intensity", intensity);
		    ComponentUI::DrawColorControl("Color", color);

		    ComponentUI::DrawFloatControl("Attenuation (Constant)", constant);
		    ComponentUI::DrawFloatControl("Attenuation (Linear)", linear);
		    ComponentUI::DrawFloatControl("Attenuation (Quadratic)", quadratic);

		    light.SetIntensity(intensity);
		    light.SetLightColor(color);
		    light.SetAttenuation(constant, linear, quadratic);
	    });

	return true;
}();

}
