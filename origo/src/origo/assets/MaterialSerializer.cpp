#include "origo/assets/MaterialSerializer.h"
#include "nlohmann/json.hpp"
#include "origo/assets/Material.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void MaterialSerializer::Serialize(const Ref<Asset>& asset, ISerializer& backend) const {
	Ref<Material> material { std::dynamic_pointer_cast<Material>(asset) };

	backend.Write("shader", material->GetShader()->GetId().ToString());
	material->GetUniformList()->Serialize(backend);
	backend.Write("albedo", material->GetAlbedo()->GetId().ToString());
}

Ref<Asset> MaterialSerializer::Deserialize(ISerializer& backend) const {
	// TODO:
	return nullptr;
}

REGISTER_SERIALIZER(Material)
}
