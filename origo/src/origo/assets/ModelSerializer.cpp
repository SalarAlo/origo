#include "origo/assets/ModelSerializer.h"
#include "origo/serialization/ISerializer.h"

// TODO

namespace Origo {

void ModelSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type model");
}

Scope<Asset> ModelSerializer::Deserialize(ISerializer& backend) const {
	throw std::logic_error("Not implemented");
}

}
