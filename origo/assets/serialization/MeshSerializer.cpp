#include "origo/assets/serialization/MeshSerializer.h"

#include "origo/assets/Mesh.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void MeshSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	throw std::logic_error("Not implemented");
}

void MeshSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto mesh { dynamic_cast<const Mesh*>(asset) };
	throw std::logic_error("Not implemented");
}

}
