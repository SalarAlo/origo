#include "origo/assets/AssetManager.h"
#include "origo/assets/Asset.h"

namespace Origo {

Asset* AssetManager::Get(const UUID& id) {
	auto it { s_Records.find(id) };
	return it != s_Records.end() ? it->second.AssetPtr.get() : nullptr;
}

UUID AssetManager::Register(Scope<Asset>&& asset, std::optional<UUID> uuid, const std::string& path) {
	UUID id { (uuid.has_value() ? uuid.value() : UUID::Generate()) };
	Record rec { .AssetPtr = std::move(asset), .Uuid = id };
	if (!path.empty()) {
		rec.Path = path;
		s_PathToAsset[rec.Path] = id;
	}

	s_Records[rec.Uuid] = std::move(rec);
	return id;
}

bool AssetManager::HasAssetWithPath(const std::string& view) {
	return s_PathToAsset.contains(view);
}

UUID AssetManager::GetAssetByPath(const std::string& view) {
	auto pathToAssetEntry { s_PathToAsset.find(view) };
	return s_PathToAsset.end() == pathToAssetEntry ? UUID::Bad() : pathToAssetEntry->second;
}

}
