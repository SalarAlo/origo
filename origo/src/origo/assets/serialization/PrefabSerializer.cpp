#include "origo/assets/serialization/PrefabSerializer.h"

#include "origo/assets/Prefab.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void PrefabSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto prefab { dynamic_cast<const Prefab*>(asset) };

	prefab->backend.write_to(backend);
}

void PrefabSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto prefab { dynamic_cast<Prefab&>(asset) };
	backend.write_to(prefab.backend);
}

}
