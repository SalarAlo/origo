#include "origo/assets/AssetSerializer.h"

#include "origo/assets/AssetSerializer.h"
#include "origo/assets/AssetManager.h"

#include "nlohmann/json.hpp"

namespace Origo::AssetSerializationSystem {

static std::unordered_map<AssetType, Ref<AssetSerializer>>& GetRegistry() {
	static std::unordered_map<AssetType, Ref<AssetSerializer>> registry;
	return registry;
}

void Register(AssetType type, Ref<AssetSerializer> serializer) {
	auto& reg = GetRegistry();
	reg[type] = std::move(serializer);
}

Ref<AssetSerializer> Get(AssetType type) {
	auto& reg = GetRegistry();
	auto it = reg.find(type);
	if (it != reg.end())
		return it->second;
	return nullptr;
}

nlohmann::json SaveAll() {
	nlohmann::json serialized = nlohmann::json::array();

	for (const auto& [id, record] : AssetManager::GetRecords()) {
		if (!record.AssetReference->ShouldSerialize())
			continue;
		const auto& asset { record.AssetReference };
		auto serializer { AssetSerializationSystem::Get(asset->GetAssetType()) };
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

nlohmann::json LoadAll() {
	return {};
}

}
