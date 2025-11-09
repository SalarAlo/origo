#include "origo/assets/ModelSerializer.h"
#include "origo/assets/Model.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void ModelSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto model { static_cast<const Model*>(asset) };

	backend.Write("path", model->GetPath());
}

Asset* ModelSerializer::Deserialize(ISerializer& backend) const {
	// TODO
	return nullptr;
}

REGISTER_SERIALIZER(Model)

}
