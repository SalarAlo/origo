
#include "origo/assets/ShaderSerializer.h"
#include "origo/assets/ShaderSourceSerializer.h"
#include "origo/assets/Shader.h"

namespace Origo {

void ShaderSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	const auto shader { dynamic_cast<const Shader*>(asset) };
	ShaderSourceSerializer::Serialize(shader->GetSource(), backend);
}

Asset* ShaderSerializer::Deserialize(ISerializer& backend) const {
	return nullptr;
}

REGISTER_SERIALIZER(Shader)
}
