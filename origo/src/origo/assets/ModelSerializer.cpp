#include "origo/assets/ModelSerializer.h"
#include "origo/serialization/ISerializer.h"

// TODO

namespace Origo {

void ModelSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
}

Asset* ModelSerializer::Deserialize(ISerializer& backend) const {
	return nullptr;
}

REGISTER_SERIALIZER(Model)

}
