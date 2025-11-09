#include "origo/assets/MeshSerializer.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/Mesh.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {
void MeshSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	const auto mesh { dynamic_cast<const Mesh*>(asset) };
	mesh->GetSource()->Serialize(backend);
}

Asset* MeshSerializer::Deserialize(ISerializer& backend) const {
	// TODO
	return nullptr;
}

REGISTER_SERIALIZER(Mesh)
}
