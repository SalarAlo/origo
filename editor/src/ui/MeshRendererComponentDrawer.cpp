#include <glm/vec3.hpp>

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

#include "origo/components/MeshRendererComponent.h"

#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"

namespace OrigoEditor {

static bool s_registered = []() {
	InspectorDrawRegistry::get_instance().register_native_drawer<Origo::MeshRendererComponent>("Mesh Renderer", "icons/Paint.svg", [](Origo::MeshRendererComponent& renderer) {
		auto& am { Origo::AssetManager::get_instance() };

		auto material { renderer.MaterialHandle };
		auto mesh { renderer.MeshHandle };

		ComponentUI::draw_asset_control("Material", material, Origo::AssetType::MaterialPBR);
		ComponentUI::draw_asset_control("Mesh", mesh, Origo::AssetType::Mesh);

		if (mesh != renderer.MeshHandle && mesh.has_value())
			renderer.MeshHandle = mesh;
		if (material != renderer.MaterialHandle && material.has_value())
			renderer.MaterialHandle = material;
	});

	return true;
}();

}
