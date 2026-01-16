#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"
#include "origo/components/ModelRenderer.h"
#include "origo/components/ModelRenderer.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::ModelRendererComponent>("Model Renderer", "icons/Paint.svg", [](Origo::ModelRendererComponent& renderer) {
		auto& am { Origo::AssetManager::GetInstance() };

		auto mesh { renderer.GetModel() };

		ComponentUI::DrawAssetControl("Model", mesh, Origo::AssetType::Model);

		if (mesh != renderer.GetModel() && mesh.has_value())
			renderer.SetModel(*mesh);
	});

	return true;
}();

}
