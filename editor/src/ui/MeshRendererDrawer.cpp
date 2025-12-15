#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Material.h"
#include "origo/scene/MeshRenderer.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawableRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::Register<Origo::MeshRenderer>("Mesh Renderer", [](Origo::MeshRenderer& renderer) {
		auto& am { Origo::AssetManagerFast::GetInstance() };
		auto material { am.Get<Origo::Material>(renderer.GetMaterial()) };

		auto shaderId { am.GetUUID(material->GetShader()).ToString() };
		auto materialId { am.GetUUID(renderer.GetMaterial()).ToString() };
		auto albedoId { am.GetUUID(material->GetAlbedo()).ToString() };
		auto meshId { am.GetUUID(renderer.GetMesh()).ToString() };

		ComponentUI::DrawStringControl("Material", materialId);
		ComponentUI::DrawStringControl("Shader", shaderId);
		ComponentUI::DrawStringControl("Albedo", albedoId);
		ComponentUI::DrawStringControl("Mesh", meshId);
	});

	return true;
}();

}
