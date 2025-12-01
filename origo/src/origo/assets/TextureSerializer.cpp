#include "origo/assets/TextureSerializer.h"
#include "magic_enum/magic_enum.hpp"
#include "origo/assets/Texture.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void TextureSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	auto texture { dynamic_cast<const Texture*>(asset) };

	texture->GetSource()->Serialize(backend);
	backend.Write("texture_type", magic_enum::enum_name(texture->GetTextureType()));
}

Asset* TextureSerializer::Deserialize(ISerializer& backend) const {
	// TODO
	return nullptr;
}

REGISTER_SERIALIZER(Texture)

}
