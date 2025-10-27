#include "origo/assets/TextureSerializer.h"
#include "origo/assets/AssetManager.h"
#include "magic_enum/magic_enum.hpp"
#include "origo/assets/Texture.h"
#include "origo/serialization/ISerializer.h"
#include <memory>

namespace Origo {

void TextureSerializer::Serialize(const Ref<Asset>& asset, ISerializer& backend) const {
	Ref<Texture> texture { std::dynamic_pointer_cast<Texture>(asset) };

	texture->GetSource()->Serialize(backend);
	backend.Write("texture_type", magic_enum::enum_name(texture->GetTextureType()));
}

Ref<Asset> TextureSerializer::Deserialize(ISerializer& backend) const {
	// todo
	return nullptr;
}

REGISTER_SERIALIZER(Texture)

}
