#include "origo/assets/serialization/MaterialSerializer.h"

#include "origo/assets/Material2D.h"
#include "origo/assets/Material2DSource.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void MaterialSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto material {
		static_cast<const Material2D*>(asset)
	};

	auto source = material->get_source();

	if (!source) {
		ORG_CORE_ERROR("Material has no source — cannot serialize");
		return;
	}

	source->serialize(backend);

	ORG_CORE_TRACE("Serializing an asset of type material");
}

void MaterialSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto& material { static_cast<Material2D&>(asset) };
	auto deserialized = Material2DSource::deserialize(backend);

	if (!deserialized) {
		return;
	}

	// no need to call resolve since importer pipeline
	// will do so later
	material.set_source(std::move(deserialized));
}

}
