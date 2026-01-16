#include "ModelRendererComponentSerializer.h"
#include "origo/assets/AssetManager.h"
#include "origo/components/ModelRenderer.h"

namespace Origo {

void ModelRendererComponentSerializer::Serialize(Component* comp, ISerializer& backend) const {
	auto renderer = static_cast<ModelRendererComponent*>(comp);

	auto model = renderer->GetModel();
	if (!model.has_value())
		return;

	auto uuid = AssetManager::GetInstance().GetUUID(*model);
	if (!uuid.has_value())
		return;

	backend.Write("model", uuid->ToString());
}

void ModelRendererComponentSerializer::Deserialize(Component* comp, ISerializer& backend) {
	auto renderer = static_cast<ModelRendererComponent*>(comp);

	std::string modelId;
	if (!backend.TryRead("model", modelId))
		return;

	auto uuid = UUID::FromString(modelId);
	auto handle = AssetManager::GetInstance().GetHandleByUUID(uuid);
	if (!handle.has_value())
		return;

	renderer->SetModel(*handle);
}

}
