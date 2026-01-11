#include "ModelRenderer.h"
#include "origo/scene/NativeComponentRegistry.h"

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::ModelRenderer);

namespace Origo {

void ModelRenderer::Serialize(ISerializer& backend) const {
	if (!m_Model.has_value())
		return;

	auto uuid = AssetManager::GetInstance().GetUUID(*m_Model);
	if (!uuid.has_value())
		return;

	backend.Write("model", uuid->ToString());
}

void ModelRenderer::Deserialize(ISerializer& backend) {
	std::string modelId;
	if (!backend.TryRead("model", modelId))
		return;

	UUID uuid = UUID::FromString(modelId);

	auto handle = AssetManager::GetInstance().GetHandleByUUID(uuid);
	if (!handle.has_value())
		return;

	m_Model = handle;
}

}
