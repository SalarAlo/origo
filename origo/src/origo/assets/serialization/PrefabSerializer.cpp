#include "origo/assets/serialization/PrefabSerializer.h"

#include "origo/assets/Prefab.h"

#include "origo/serialization/ISerializer.h"
#include "origo/serialization/JsonSerializer.h"

namespace Origo {

void PrefabSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto prefab { dynamic_cast<const Prefab*>(asset) };

	prefab->backend.write_to(backend);
}

void PrefabSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto& prefab = dynamic_cast<Prefab&>(asset);
	auto* json_backend = dynamic_cast<JsonSerializer*>(&backend);
	if (!json_backend) {
		ORG_ERROR("PrefabSerializer::deserialize requires a JsonSerializer source");
		return;
	}

	json_backend->write_to(prefab.backend);
}

}
