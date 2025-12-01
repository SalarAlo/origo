#pragma once

#include "origo/assets/Asset.h"
#include "origo/core/RID.h"
#include <concepts>
#include <unordered_map>

namespace Origo {

template <typename T>
concept AssetConcept = std::derived_from<T, Asset> && requires(T t) {
	{ t.GetClassAssetType() } -> std::same_as<AssetType>;
};

class AssetManager {
private:
	struct Record;

public:
	static Asset* GetAssetChecked(const RID& id);
	template <AssetConcept T>
	static void Register(Scope<T>&& asset) {
		s_Records[asset->GetId()] = std::move(asset);
	}

	template <AssetConcept T>
	static T* GetAssetAs(RID id) {
		auto it { s_Records.find(id) };

		if (it == s_Records.end()) {
			ORG_INFO("Could not find {} Asset with runtime ID", id.ToString());
			return nullptr;
		}

		return static_cast<T*>(it->second.get());
	}

	static const std::unordered_map<RID, Scope<Asset>>& GetRecords() { return s_Records; }

private:
	inline static std::unordered_map<RID, Scope<Asset>> s_Records {};
};
}
