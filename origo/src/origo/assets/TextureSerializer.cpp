#include "origo/assets/TextureSerializer.h"
#include "magic_enum/magic_enum.hpp"
#include "origo/assets/Texture.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void TextureSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto texture { dynamic_cast<const Texture*>(asset) };

	backend.Write("texture_type", magic_enum::enum_name(texture->GetTextureType()));
	texture->GetSource()->Serialize(backend);
	ORG_INFO("Seriliazing an asset of type texture");
}

void TextureSerializer::Deserialize(ISerializer& backend, Asset& asset) const {
	std::string typeStr {};
	backend.TryRead("texture_type", typeStr);
	auto optionalType { magic_enum::enum_cast<TextureType>(typeStr) };
	if (!optionalType.has_value()) {
		ORG_ERROR("TextureSerializer: Unknown texture type '{}'", typeStr);
	}
	auto type { optionalType.value() };
	auto source { TextureSource::Deserialize(backend) };
	auto& texture { static_cast<Texture&>(asset) };

	texture.SetTextureType(type);
	texture.SetSource(std::move(source));
}

}
