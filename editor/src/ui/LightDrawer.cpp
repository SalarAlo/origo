#include "origo/assets/Asset.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Light.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::Light>("Light", "icons/Light.svg", [](Origo::Light& light) {
		auto& am { Origo::AssetManager::GetInstance() };

		auto target { light.GetShaderTarget() };
		float ambient { light.GetAmbientFactor() };
		int shinyFactor { light.GetShinyFactor() };
		auto lightColor { light.GetLightColor() };

		ComponentUI::DrawAssetControl("Target", target, Origo::AssetType::Shader);
		ComponentUI::DrawFloatControl("Ambient", ambient);
		ComponentUI::DrawIntControl("Shiny", shinyFactor);
		ComponentUI::DrawVec3Control("Color", lightColor);

		light.SetLightColor(lightColor);
		light.SetAmbientFactor(ambient);
		light.SetShinyFactor(shinyFactor);
		if (target.has_value())
			light.SetShaderTarget(*target);
	});

	return true;
}();

}
