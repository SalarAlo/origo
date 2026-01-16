#include "MeshRendererComponentSerializer.h"
#include "origo/assets/AssetManager.h"
#include "origo/components/MeshRenderer.h"

namespace Origo {

void MeshRendererComponentSerializer::Serialize(Component* comp, ISerializer& backend) const {
	auto renderer = static_cast<MeshRendererComponent*>(comp);

	if (renderer->GetMaterial().has_value()) {
		auto uuid = AssetManager::GetInstance().GetUUID(*renderer->GetMaterial());
		if (uuid.has_value()) {
			backend.Write("material", uuid->ToString());
		}
	}

	if (renderer->GetMesh().has_value()) {
		auto uuid = AssetManager::GetInstance().GetUUID(*renderer->GetMesh());
		if (uuid.has_value()) {
			backend.Write("mesh", uuid->ToString());
		}
	}
}

void MeshRendererComponentSerializer::Deserialize(Component* comp, ISerializer& backend) {
	auto renderer = static_cast<MeshRendererComponent*>(comp);

	std::string materialId;
	if (backend.TryRead("material", materialId)) {
		auto uuid = UUID::FromString(materialId);
		auto handle = AssetManager::GetInstance().GetHandleByUUID(uuid);
		if (handle.has_value()) {
			renderer->SetMaterial(*handle);
		}
	}

	std::string meshId;
	if (backend.TryRead("mesh", meshId)) {
		auto uuid = UUID::FromString(meshId);
		auto handle = AssetManager::GetInstance().GetHandleByUUID(uuid);
		if (handle.has_value()) {
			renderer->SetMesh(*handle);
		}
	}
}

}
