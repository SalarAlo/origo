#include "MeshRendererComponentSerializer.h"

#include "origo/assets/AssetManager.h"

#include "origo/components/MeshRendererComponent.h"

namespace Origo {

void MeshRendererComponentSerializer::serialize(Component* comp, ISerializer& backend) const {
	auto renderer = static_cast<MeshRendererComponent*>(comp);

	if (renderer->MaterialHandle.has_value()) {
		auto uuid = AssetManager::get_instance().get_uuid(*renderer->MaterialHandle);
		if (uuid.has_value()) {
			backend.write("material", uuid->to_string());
		}
	}

	if (renderer->MeshHandle.has_value()) {
		auto uuid = AssetManager::get_instance().get_uuid(*renderer->MeshHandle);
		if (uuid.has_value()) {
			backend.write("mesh", uuid->to_string());
		}
	}
}

void MeshRendererComponentSerializer::deserialize(Component* comp, ISerializer& backend) {
	auto renderer = static_cast<MeshRendererComponent*>(comp);

	std::string material_id;
	if (backend.try_read("material", material_id)) {
		auto uuid = UUID::from_string(material_id);
		renderer->MaterialHandle = AssetManager::get_instance().get_handle_by_uuid(uuid);
	}

	std::string mesh_id;
	if (backend.try_read("mesh", mesh_id)) {
		auto uuid = UUID::from_string(mesh_id);
		renderer->MeshHandle = AssetManager::get_instance().get_handle_by_uuid(uuid);
	}
}

}
