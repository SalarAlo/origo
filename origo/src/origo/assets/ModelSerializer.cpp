#include "origo/assets/ModelSerializer.h"
#include "nlohmann/json.hpp"
#include "origo/assets/Model.h"
#include <memory>

namespace Origo {

nlohmann::json ModelSerializer::Serialize(const Ref<Asset>& asset) const {
	auto model { std::dynamic_pointer_cast<Model>(asset) };
	nlohmann::json j;

	j["path"] = model->GetPath();
	return j;
}

Ref<Asset> ModelSerializer::Deserialize(const nlohmann::json& j) const {
	return nullptr;
}

REGISTER_SERIALIZER(Model)

}
