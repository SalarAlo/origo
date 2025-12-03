
#include "origo/assets/ShaderSerializer.h"

// TODO

namespace Origo {

void ShaderSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type shader");
}

Asset* ShaderSerializer::Deserialize(ISerializer& backend) const {
	throw std::logic_error("Not implemented");
}

}
