#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetSerializer.h"
#include "nlohmann/json.hpp"
#include <unordered_map>

namespace Origo {

Ref<Asset> AssetManager::GetAsset(UUID id) {
	auto it { s_Records.find(id) };
	return it != s_Records.end() ? it->second.AssetReference : nullptr;
}

nlohmann::json AssetManager::SaveAll() {
	nlohmann::json serialized = nlohmann::json::array();

	for (const auto& [id, record] : s_Records) {
		if (!record.AssetReference->ShouldSerialize())
			continue;
		const auto& asset { record.AssetReference };
		auto serializer { AssetSerializationRegistry::Get(asset->GetAssetType()) };
		nlohmann::json toAdd;

		if (serializer == nullptr) {
			ORG_INFO("Asset of type {} has no serializer", magic_enum::enum_name(asset->GetAssetType()));
		} else {
			ORG_INFO("Asset of type {} has a serializer", magic_enum::enum_name(asset->GetAssetType()));
			toAdd = serializer->Serialize(asset);
		}

		toAdd["name"] = record.Name;
		toAdd["id"] = asset->GetId().ToString();
		toAdd["asset_type"] = magic_enum::enum_name(asset->GetAssetType());

		serialized.push_back(toAdd);
	}

	return serialized;
}

static nlohmann::json LoadAll() {
	return {};
}
}
