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
	auto rgb { material->get_color() };

	backend.write("color_r", rgb.r);
	backend.write("color_g", rgb.g);
	backend.write("color_b", rgb.b);

	material->get_uniform_list().serialize(backend);
}

void MaterialSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	auto& material { static_cast<Material2D&>(asset) };

	auto deserialized = Material2DSource::deserialize(backend);

	if (!deserialized) {
		ORG_CORE_ERROR("failed to deserialize material");
		return;
	}

	material.set_source(std::move(deserialized));

	Vec3 rgb { 1.0 };
	backend.try_read("color_r", rgb.r);
	backend.try_read("color_g", rgb.g);
	backend.try_read("color_b", rgb.b);

	material.get_uniform_list().deserialize(backend);

	material.set_color(rgb);
}

}
