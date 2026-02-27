#include <glm/vec3.hpp>

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

#include "origo/components/ModelRenderer.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::ModelRendererComponent>("Model Renderer", "icons/Paint.svg", [](Origo::ModelRendererComponent& renderer) {
		auto& am { Origo::AssetManager::get_instance() };

		auto handle { renderer.ModelHandle };

		ComponentUI::draw_asset_control("Model", handle, Origo::AssetType::Model);

		if (handle != renderer.ModelHandle && handle.has_value())
			renderer.ModelHandle = handle;
	});

	return true;
}();

}
