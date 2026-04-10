#include "origo/assets/AssetManager.h"

#include "origo/components/DirectionalLightComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::DirectionalLightComponent>("DirectionalLight", "icons/Light.svg", [](Origo::DirectionalLightComponent& light) {
		auto& am { Origo::AssetManager::get_instance() };

		ComponentUI::draw_float_control("Ambient", light.AmbientFactor);
		ComponentUI::draw_float_control("Intensity", light.Intensity);
		ComponentUI::draw_color_control("Color", light.LightColor);
	});

	return true;
}();

}
