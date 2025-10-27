#include "origo/assets/ModelSerializer.h"
#include "origo/assets/Model.h"
#include "origo/serialization/ISerializer.h"
#include <memory>

namespace Origo {

void ModelSerializer::Serialize(const Ref<Asset>& asset, ISerializer& backend) const {
	auto model { std::dynamic_pointer_cast<Model>(asset) };

	backend.Write("path", model->GetPath());
}

Ref<Asset> ModelSerializer::Deserialize(ISerializer& backend) const {
	// TODO
	return nullptr;
}

REGISTER_SERIALIZER(Model)

}
