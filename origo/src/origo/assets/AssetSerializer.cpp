#include "origo/assets/AssetSerializer.h"

#include "origo/assets/AssetSerializer.h"
#include "origo/assets/AssetManager.h"
#include "origo/core/Logger.h"
#include "origo/serialization/ISerializer.h"

namespace Origo::AssetSerializationSystem {

static std::unordered_map<AssetType, AssetSerializer*>& GetRegistry() {
	static std::unordered_map<AssetType, AssetSerializer*> registry;
	return registry;
}

void Register(AssetType type, AssetSerializer* serializer) {
	auto& reg = GetRegistry();
	reg[type] = serializer;
}

void Cleanup() {
	for (const auto& kv : GetRegistry()) {
		delete kv.second;
	}
}

AssetSerializer* Get(AssetType type) {
	auto& reg = GetRegistry();
	auto it = reg.find(type);
	if (it != reg.end())
		return it->second;
	return nullptr;
}

void SaveAll(ISerializer& backend) {
	backend.BeginArray("assets");

	for (const auto& [id, record] : AssetManager::GetRecords()) {
		const auto& asset { record.AssetReference.get() };
		auto assetSerializer { AssetSerializationSystem::Get(asset->GetAssetType()) };

		if (!assetSerializer) {
			ORG_INFO("Asset of type {} has no serializer", magic_enum::enum_name(asset->GetAssetType()));
			continue;
		}

		if (!assetSerializer->ShouldSerialize(asset))
			continue;

		backend.BeginArrayElement();

		assetSerializer->Serialize(asset, backend);

		backend.Write("asset_type", static_cast<int>(asset->GetAssetType()));
		backend.Write("name", record.Name);
		backend.Write("id", asset->GetId().ToString());

		backend.EndArrayElement();
	}

	backend.EndArray();
}

void LoadAll(ISerializer& backend) {
	// Todo:
}

}
