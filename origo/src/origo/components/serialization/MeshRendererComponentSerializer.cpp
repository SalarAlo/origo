#include "MeshRendererComponentSerializer.h"

#include "origo/assets/AssetManager.h"

#include "origo/components/MeshRenderer.h"

namespace Origo {

void MeshRendererComponentSerializer::Serialize(Component* comp, ISerializer& backend) const {
	auto renderer = static_cast<MeshRendererComponent*>(comp);

	if (renderer->MaterialHandle.has_value()) {
		auto uuid = AssetManager::GetInstance().GetUUID(*renderer->MaterialHandle);
		if (uuid.has_value()) {
			backend.Write("material", uuid->ToString());
		}
	}

	if (renderer->MeshHandle.has_value()) {
		auto uuid = AssetManager::GetInstance().GetUUID(*renderer->MeshHandle);
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
		renderer->MaterialHandle = AssetManager::GetInstance().GetHandleByUUID(uuid);
	}

	std::string meshId;
	if (backend.TryRead("mesh", meshId)) {
		auto uuid = UUID::FromString(meshId);
		renderer->MeshHandle = AssetManager::GetInstance().GetHandleByUUID(uuid);
	}
}

}
