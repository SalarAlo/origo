#include "origo/assets/AssetManager.h"
#include "origo/components/DirectionalLight.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::DirectionalLightComponent>("DirectionalLight", "icons/Light.svg", [](Origo::DirectionalLightComponent& light) {
		auto& am { Origo::AssetManager::GetInstance() };

		float ambient { light.GetAmbientFactor() };
		float shinyFactor { light.GetIntensity() };
		auto lightColor { light.GetLightColor() };

		ComponentUI::DrawFloatControl("Ambient", ambient);
		ComponentUI::DrawFloatControl("Intensity", shinyFactor);
		ComponentUI::DrawColorControl("Color", lightColor);

		light.SetLightColor(lightColor);
		light.SetAmbientFactor(ambient);
		light.SetIntensity(shinyFactor);
	});

	return true;
}();

}
