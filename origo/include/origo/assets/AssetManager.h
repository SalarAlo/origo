#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/AssetMetadata.h"
#include "origo/core/RID.h"
#include <concepts>
#include <unordered_map>

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
	struct Record;

public:
	static Asset* GetAsset(const RID& id);
	static void Register(Scope<Asset>&& asset, const UUID* uuid = nullptr) {
		auto rid { asset->GetId() };
		s_Records[rid] = std::move(asset);

		if (uuid)
			s_RIDToUUID[rid] = *uuid;
	}

	template <AssetConcept T>
	static T* GetAssetAs(RID id) {
		auto it { s_Records.find(id) };

		if (it == s_Records.end()) {
			ORG_INFO("Could not find {} Asset with runtime ID", id.ToString());
			return nullptr;
		}

		return dynamic_cast<T*>(it->second.get());
	}

	static const std::unordered_map<RID, Scope<Asset>>& GetRecords() { return s_Records; }
	static bool HasUUID(RID rid) {
		return s_RIDToUUID.contains(rid);
	}

	static UUID GetUUID(RID rid) {
		return s_RIDToUUID.at(rid);
	}

private:
	inline static std::unordered_map<RID, Scope<Asset>> s_Records {};
	inline static std::unordered_map<RID, UUID> s_RIDToUUID {};
};
}
