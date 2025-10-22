#include "origo/assets/TextureSerializer.h"
#include "origo/assets/AssetManager.h"
#include "magic_enum/magic_enum.hpp"
#include "nlohmann/json.hpp"
#include "origo/assets/Texture.h"
#include "origo/core/Logger.h"
#include <memory>

namespace Origo {

nlohmann::json TextureSerializer::Serialize(const Ref<Asset>& asset) const {
	Ref<Texture> texture { std::dynamic_pointer_cast<Texture>(asset) };
	nlohmann::json serializedTexture;
	serializedTexture["path"] = texture->GetPath();
	serializedTexture["texture_type"] = magic_enum::enum_name(texture->GetAssetType());
	return serializedTexture;
}

Ref<Asset> TextureSerializer::Deserialize(const nlohmann::json& j) const {
	std::string path { j["path"] };
	std::string typeName { j["texture_type"] };
	std::string name { j["name"] };
	std::optional<TextureType> textureType { magic_enum::enum_cast<TextureType>(typeName) };
	if (textureType.has_value()) {
		TextureType t { textureType.value() };
		return AssetManager::CreateAsset<Texture>(name, path, t);
	} else {
		ORG_CORE_ERROR("Texture type of name {} non existent in codebase", typeName);
	}

	return nullptr;
}

}
