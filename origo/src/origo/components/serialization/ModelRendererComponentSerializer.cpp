#include "ModelRendererComponentSerializer.h"

#include "origo/assets/AssetManager.h"

#include "origo/components/ModelRendererComponent.h"

namespace Origo {

void ModelRendererComponentSerializer::serialize(Component* comp, ISerializer& backend) const {
	auto renderer = static_cast<ModelRendererComponent*>(comp);

	auto model = renderer->ModelHandle;
	if (!model.has_value())
		return;

	auto uuid = AssetManager::get_instance().get_uuid(*model);
	if (!uuid.has_value())
		return;

	backend.write("model", uuid->to_string());
}

void ModelRendererComponentSerializer::deserialize(Component* comp, ISerializer& backend) {
	auto renderer = static_cast<ModelRendererComponent*>(comp);

	std::string model_id;
	if (!backend.try_read("model", model_id))
		return;

	auto uuid = UUID::from_string(model_id);
	renderer->ModelHandle = AssetManager::get_instance().get_handle_by_uuid(uuid);
}

}
