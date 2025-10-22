#include "origo/assets/MaterialSerializer.h"
#include "nlohmann/json.hpp"
#include "origo/assets/Material.h"

namespace Origo {

nlohmann::json MaterialSerializer::Serialize(const Ref<Asset>& asset) const {
	nlohmann::json j;
	Ref<Material> material { std::dynamic_pointer_cast<Material>(asset) };

	j["shader"] = material->GetShader()->GetId().ToString();
	j["uniform_list"] = material->GetUniformList()->Serialize();
	j["albedo"] = material->GetAlbedo()->GetId().ToString();

	return j;
}

Ref<Asset> MaterialSerializer::Deserialize(const nlohmann::json& j) const {
	return nullptr;
}
}
