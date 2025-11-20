
#include "origo/assets/ShaderSerializer.h"

// TODO

namespace Origo {

void ShaderSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
}

Asset* ShaderSerializer::Deserialize(ISerializer& backend) const {
	return nullptr;
}

REGISTER_SERIALIZER(Shader)
}
