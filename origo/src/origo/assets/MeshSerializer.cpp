#include "origo/assets/MeshSerializer.h"
#include "origo/assets/Mesh.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void MeshSerializer::Deserialize(ISerializer& backend, Asset& asset) const {
	// TODO
	throw std::logic_error("Not implemented");
}

void MeshSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto mesh { dynamic_cast<const Mesh*>(asset) };

	// TODO
	ORG_INFO("Serializing an asset of type mesh");
}

}
