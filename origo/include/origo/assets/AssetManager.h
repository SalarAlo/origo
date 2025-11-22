#pragma once

#include "origo/assets/Asset.h"
#include <concepts>
#include <string>
#include <unordered_map>

namespace Origo {

template <typename T>
concept AssetConcept = std::derived_from<T, Asset>;
class AssetManager {
private:
	struct Record;

public:
	template <AssetConcept T, typename... Args>
	static RID CreateAsset(const std::string& name, Args&&... args) {
		auto asset { MakeScope<T>(std::forward<Args>(args)...) };
		RID id = asset->GetId();
		s_Records[id] = { name, std::move(asset) };

		return id;
	}

	static const std::unordered_map<RID, Record>& GetRecords() { return s_Records; }

	static Asset* GetAssetChecked(const RID& id);

	template <AssetConcept T>
	static T* GetAssetAs(RID id) {
		auto it { s_Records.find(id) };

		if (it == s_Records.end()) {
			ORG_INFO("Could not find {} Asset with runtime ID", id.ToString());
			return nullptr;
		}

		return static_cast<T*>(it->second.AssetReference.get());
	}

private:
	struct Record {
		std::string Name {};
		Scope<Asset> AssetReference {};
	};

private:
	inline static std::unordered_map<RID, Record> s_Records {};
};
}
