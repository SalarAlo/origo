#include "origo/assets/AssetManager.h"
#include "origo/assets/Material.h"
#include "origo/scene/MeshRenderer.h"
#include "ui/ComponentUI.h"
#include "ui/InspectorDrawableRegistry.h"
#include <glm/vec3.hpp>

namespace OrigoEditor {

static bool s_Registered = []() {
	InspectorDrawRegistry::Register<Origo::MeshRenderer>("Mesh Renderer", [](Origo::MeshRenderer& t) {
		auto material { Origo::AssetManager::GetAssetAs<Origo::Material>(t.GetMaterial()) };

		auto shaderId { material->GetShader().ToString() };
		auto materialId { t.GetMaterial().ToString() };
		auto albedoId { material->GetAlbedo().ToString() };
		auto meshId { t.GetMesh().ToString() };

		ComponentUI::DrawStringControl("Material", materialId);
		ComponentUI::DrawStringControl("Shader", shaderId);
		ComponentUI::DrawStringControl("Albedo", albedoId);
		ComponentUI::DrawStringControl("Mesh", meshId);
	});

	return true;
}();

}
