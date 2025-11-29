#include "origo/assets/MeshSerializer.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/Mesh.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {
void MeshSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	const auto mesh { dynamic_cast<const Mesh*>(asset) };
	// TODO
}

Asset* MeshSerializer::Deserialize(ISerializer& backend) const {
	// TODO
	return nullptr;
}

bool MeshSerializer::ShouldSerialize(const Asset* asset) const {
	auto mesh { dynamic_cast<const Mesh*>(asset) };

	// TODO
	return true;
}

REGISTER_SERIALIZER(Mesh)
}
