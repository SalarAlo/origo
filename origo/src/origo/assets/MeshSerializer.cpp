#include "origo/assets/MeshSerializer.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/assets/Mesh.h"
#include "origo/serialization/ISerializer.h"
#include <memory>

namespace Origo {
void MeshSerializer::Serialize(const Ref<Asset>& asset, ISerializer& backend) const {
	const auto& mesh { std::dynamic_pointer_cast<Mesh>(asset) };
	mesh->GetSource()->Serialize(backend);
}

Ref<Asset> MeshSerializer::Deserialize(ISerializer& backend) const {
	// TODO
	return nullptr;
}

REGISTER_SERIALIZER(Mesh)
}
