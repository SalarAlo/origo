
#include "origo/assets/ShaderSerializer.h"
#include "origo/assets/ShaderSourceSerializer.h"
#include "origo/assets/Shader.h"

namespace Origo {

void ShaderSerializer::Serialize(const Ref<Asset>& asset, ISerializer& backend) const {
	Ref<Shader> shader = std::dynamic_pointer_cast<Shader>(asset);
	ShaderSourceSerializer::Serialize(shader->GetSource(), backend);
}

Ref<Asset> ShaderSerializer::Deserialize(ISerializer& backend) const {
	return nullptr;
}

REGISTER_SERIALIZER(Shader)
}
