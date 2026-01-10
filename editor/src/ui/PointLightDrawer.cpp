#include "origo/assets/Asset.h"
#include "origo/scene/PointLight.h"
#include "origo/scene/SystemScheduler.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::PointLight>(
	    "PointLight",
	    "icons/Light.svg",
	    [](Origo::PointLight& light) {
		    auto target { light.GetShaderTarget() };
		    float intensity { light.GetIntensity() };
		    auto color { light.GetLightColor() };

		    float constant { light.Constant() };
		    float linear { light.Linear() };
		    float quadratic { light.Quadratic() };

		    ComponentUI::DrawAssetControl("Target", target, Origo::AssetType::Shader);
		    ComponentUI::DrawFloatControl("Intensity", intensity);
		    ComponentUI::DrawColorControl("Color", color);

		    ComponentUI::DrawFloatControl("Attenuation (Constant)", constant);
		    ComponentUI::DrawFloatControl("Attenuation (Linear)", linear);
		    ComponentUI::DrawFloatControl("Attenuation (Quadratic)", quadratic);

		    light.SetIntensity(intensity);
		    light.SetLightColor(color);
		    light.SetAttenuation(constant, linear, quadratic);

		    if (target.has_value())
			    light.SetShaderTarget(*target);
	    });

	return true;
}();

}
