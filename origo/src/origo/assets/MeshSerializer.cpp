#include "origo/assets/MeshSerializer.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/Mesh.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

Asset* MeshSerializer::Deserialize(ISerializer& backend) const {
	// TODO
	return nullptr;
}

void MeshSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto mesh { dynamic_cast<const Mesh*>(asset) };

	// TODO
}

REGISTER_SERIALIZER(Mesh)
}
