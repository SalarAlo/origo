#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetSerializer.h"
#include "nlohmann/json.hpp"

namespace Origo {

nlohmann::json AssetManager::SaveAll() {
	nlohmann::json serialized = nlohmann::json::array();

	for (const auto& [id, asset] : s_Assets) {
		auto serializer { AssetSerializationRegistry::Get(asset->GetAssetType()) };
		nlohmann::json toAdd;

		if (serializer == nullptr) {
			ORG_INFO("Asset of type {} has no serializer", magic_enum::enum_name(asset->GetAssetType()));
		} else {
			ORG_INFO("Asset of type {} has a serializer", magic_enum::enum_name(asset->GetAssetType()));
			toAdd = serializer->Serialize(asset);
		}

		toAdd["name"] = s_AssetNames[id];
		toAdd["id"] = asset->GetId().ToString();
		serialized.push_back(toAdd);
	}

	return serialized;
}

static nlohmann::json LoadAll() {
}
}
