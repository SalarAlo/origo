#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Material.h"
#include "origo/scene/MeshRenderer.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::RegisterNativeDrawer<Origo::MeshRenderer>("Mesh Renderer", "icons/Paint.svg", [](Origo::MeshRenderer& renderer) {
		auto& am { Origo::AssetManagerFast::GetInstance() };
		auto material { am.Get<Origo::Material2D>(renderer.GetMaterial()) };

		auto materialId { am.GetUUID(renderer.GetMaterial()).ToString() };
		auto meshId { am.GetUUID(renderer.GetMesh()).ToString() };

		ComponentUI::DrawStringControl("Material", materialId);
		ComponentUI::DrawStringControl("Mesh", meshId);
	});

	return true;
}();

}
