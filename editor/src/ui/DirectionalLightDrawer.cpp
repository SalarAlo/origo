#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/scene/DirectionalLight.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::DirectionalLight>("DirectionalLight", "icons/Light.svg", [](Origo::DirectionalLight& light) {
		auto& am { Origo::AssetManager::GetInstance() };

		auto target { light.GetShaderTarget() };
		float ambient { light.GetAmbientFactor() };
		float shinyFactor { light.GetIntensity() };
		auto lightColor { light.GetLightColor() };

		ComponentUI::DrawAssetControl("Target", target, Origo::AssetType::Shader);
		ComponentUI::DrawFloatControl("Ambient", ambient);
		ComponentUI::DrawFloatControl("Intensity", shinyFactor);
		ComponentUI::DrawColorControl("Color", lightColor);

		light.SetLightColor(lightColor);
		light.SetAmbientFactor(ambient);
		light.SetIntensity(shinyFactor);
		if (target.has_value())
			light.SetShaderTarget(*target);
	});

	return true;
}();

}
