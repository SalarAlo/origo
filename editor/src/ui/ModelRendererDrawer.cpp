#include "origo/assets/Asset.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/ModelRenderer.h"
#include "origo/scene/ModelRenderer.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::ModelRenderer>("Model Renderer", "icons/Paint.svg", [](Origo::ModelRenderer& renderer) {
		auto& am { Origo::AssetManager::GetInstance() };

		auto mesh { renderer.GetModel() };

		ComponentUI::DrawAssetControl("Model", mesh, Origo::AssetType::Model);

		if (mesh != renderer.GetModel() && mesh.has_value())
			renderer.SetModel(*mesh);
	});

	return true;
}();

}
