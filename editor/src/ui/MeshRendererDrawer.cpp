#include <glm/vec3.hpp>

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

#include "origo/components/MeshRenderer.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::MeshRendererComponent>("Mesh Renderer", "icons/Paint.svg", [](Origo::MeshRendererComponent& renderer) {
		auto& am { Origo::AssetManager::GetInstance() };

		auto material { renderer.MaterialHandle };
		auto mesh { renderer.MeshHandle };

		ComponentUI::DrawAssetControl("Material", material, Origo::AssetType::Material2D);
		ComponentUI::DrawAssetControl("Mesh", mesh, Origo::AssetType::Mesh);

		if (mesh != renderer.MeshHandle && mesh.has_value())
			renderer.MeshHandle = mesh;
		if (material != renderer.MaterialHandle && material.has_value())
			renderer.MaterialHandle = material;
	});

	return true;
}();

}
