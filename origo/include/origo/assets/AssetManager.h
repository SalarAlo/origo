#pragma once

#include "origo/assets/Asset.h"
#include "origo/core/UUID.h"

namespace Origo {

template <typename T>
concept AssetConcept = std::derived_from<T, Asset> && requires(T t) {
	{ t.GetClassAssetType() } -> std::same_as<AssetType>;
};

// note that i kepp a lot of systems that are basically singletons
// as classes and not namespaces because i wanna switch to instance
// based when the time arrives
class AssetManager {
private:
	struct Record {
		Scope<Asset> AssetPtr {};
		UUID Uuid {};
	};

public:
	static Asset* GetAsset(const UUID& id);
	static UUID Register(Scope<Asset>&& asset, const UUID* uuid = nullptr) {
		UUID id { (uuid ? *uuid : UUID {}) };
		Record rec { .AssetPtr = std::move(asset), .Uuid = id };

		s_Records[rec.Uuid] = std::move(rec);
		return id;
	}

	template <AssetConcept T>
	static T* GetAssetAs(const UUID& id) {
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

	static const std::unordered_map<UUID, Record>& GetRecords() { return s_Records; }

private:
	inline static std::unordered_map<UUID, Record> s_Records {};
};
}
