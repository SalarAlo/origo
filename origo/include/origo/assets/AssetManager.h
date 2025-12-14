#pragma once

#include <ankerl/unordered_dense.h>
#include <optional>

#include "origo/assets/Asset.h"
#include "origo/core/UUID.h"

namespace Origo {

template <typename T>
concept AssetConcept = std::derived_from<T, Asset> && requires(T t) {
	{ t.GetClassAssetType() } -> std::same_as<AssetType>;
};

class AssetManager {
private:
	struct Record {
		Scope<Asset> AssetPtr {};
		UUID Uuid { UUID::Bad() };
		std::string Path { "" };
	};

public:
	template <AssetConcept T>
	static T* Get(const UUID& id) {
		auto it { s_Records.find(id) };

		if (it == s_Records.end()) {
			ORG_INFO("AssetManager::GetAssetAs: Could not find {} Asset with  UUID", id.ToString());
			return nullptr;
		}

		Asset* base = it->second.AssetPtr.get();
		if (base->GetAssetType() != T::GetClassAssetType()) {
			auto assetTypeName { magic_enum::enum_name<AssetType>(T::GetClassAssetType()) };
			ORG_INFO("AssetManager::GetAssetAs: requested rid as type {} is invalid", assetTypeName);
			return nullptr;
		}

		return static_cast<T*>(base);
	}

	static UUID Register(Scope<Asset>&& asset, std::optional<UUID> uuid = std::nullopt, const std::string& path = "");

	static Asset* Get(const UUID& id);
	static UUID GetAssetByPath(const std::string& view);
	static bool HasAssetWithPath(const std::string& view);

	static const std::unordered_map<UUID, Record>& GetRecords() { return s_Records; }

private:
	inline static std::unordered_map<UUID, Record> s_Records {};
	inline static std::unordered_map<std::string, UUID> s_PathToAsset {};
};
}
