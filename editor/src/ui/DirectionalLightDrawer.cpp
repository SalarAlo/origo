#include "origo/assets/AssetManager.h"

#include "origo/components/DirectionalLight.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::DirectionalLightComponent>("DirectionalLight", "icons/Light.svg", [](Origo::DirectionalLightComponent& light) {
		auto& am { Origo::AssetManager::GetInstance() };

		ComponentUI::DrawFloatControl("Ambient", light.AmbientFactor);
		ComponentUI::DrawFloatControl("Intensity", light.Intensity);
		ComponentUI::DrawColorControl("Color", light.LightColor);
	});

	return true;
}();

}
