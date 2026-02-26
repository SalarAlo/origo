#include "origo/assets/serialization/TextureSerializer.h"

#include "magic_enum/magic_enum.hpp"

#include "origo/assets/Texture2D.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void TextureSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	auto texture { dynamic_cast<const Texture2D*>(asset) };

	backend.write("texture_type", magic_enum::enum_name(texture->get_texture_type()));
	texture->get_source()->serialize(backend);
	ORG_INFO("Seriliazing an asset of type texture");
}

void TextureSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	std::string type_str {};
	backend.try_read("texture_type", type_str);
	auto optional_type { magic_enum::enum_cast<TextureType>(type_str) };
	if (!optional_type.has_value()) {
		ORG_ERROR("TextureSerializer: Unknown texture type '{}'", type_str);
	}
	auto type { optional_type.value() };
	auto source { TextureSource::deserialize(backend) };
	auto& texture { static_cast<Texture2D&>(asset) };

	texture.set_texture_type(type);
	texture.set_source(std::move(source));
}

}
