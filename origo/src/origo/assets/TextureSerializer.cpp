#include "origo/assets/TextureSerializer.h"
#include "origo/assets/AssetManager.h"
#include "magic_enum/magic_enum.hpp"
#include "nlohmann/json.hpp"
#include "origo/assets/Texture.h"
#include <memory>

namespace Origo {

nlohmann::json TextureSerializer::Serialize(const Ref<Asset>& asset) const {
	Ref<Texture> texture { std::dynamic_pointer_cast<Texture>(asset) };
	nlohmann::json serializedTexture;

	serializedTexture["src"] = texture->GetSource()->Serialize();
	serializedTexture["texture_type"] = magic_enum::enum_name(texture->GetAssetType());

	return serializedTexture;
}

Ref<Asset> TextureSerializer::Deserialize(const nlohmann::json& j) const {

	return nullptr;
}

REGISTER_SERIALIZER(Texture)

}
