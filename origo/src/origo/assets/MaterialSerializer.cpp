#include "origo/assets/MaterialSerializer.h"
#include "origo/assets/Material.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void MaterialSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto material { dynamic_cast<const Material*>(asset) };

	backend.Write("shader", material->GetShader()->GetId().ToString());
	material->GetUniformList()->Serialize(backend);
	backend.Write("albedo", material->GetAlbedo()->GetId().ToString());
}

Asset* MaterialSerializer::Deserialize(ISerializer& backend) const {
	// TODO:
	return nullptr;
}

REGISTER_SERIALIZER(Material)
}
