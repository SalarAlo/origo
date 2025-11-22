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

		int shaderId { material->GetShader().GetId() };
		int materialId { material->GetId().GetId() };
		int albedoId { material->GetAlbedo().GetId() };
		int meshId { t.GetMesh().GetId() };

		ComponentUI::DrawIntControl("Material", materialId);
		ComponentUI::DrawIntControl("Shader", shaderId);
		ComponentUI::DrawIntControl("Albedo", albedoId);
		ComponentUI::DrawIntControl("Mesh", meshId);
	});

	return true;
}();

}
