#include "origo/assets/Asset.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/MeshRenderer.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::MeshRenderer>("Mesh Renderer", "icons/Paint.svg", [](Origo::MeshRenderer& renderer) {
		auto& am { Origo::AssetManager::GetInstance() };

		auto material { renderer.GetMaterial() };
		auto mesh { renderer.GetMesh() };

		ComponentUI::DrawAssetControl("Material", material, Origo::AssetType::Material2D);
		ComponentUI::DrawAssetControl("Mesh", mesh, Origo::AssetType::Mesh);

		if (mesh != renderer.GetMesh() && mesh.has_value())
			renderer.SetMesh(*mesh);
		if (material != renderer.GetMaterial() && material.has_value())
			renderer.SetMaterial(*material);
	});

	return true;
}();

}
