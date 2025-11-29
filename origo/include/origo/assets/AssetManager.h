#pragma once

#include "origo/assets/Asset.h"
#include "origo/core/Action.h"
#include "origo/core/RID.h"
#include <concepts>
#include <string>
#include <unordered_map>

namespace Origo {

template <typename T>
concept AssetConcept = std::derived_from<T, Asset> && requires(T t) {
	{ t.GetClassAssetType() } -> std::same_as<AssetType>;
};

class AssetManager {
private:
	struct Record;
	using AssetCreatedCallback = std::function<void(RID, AssetType)>;

public:
	template <AssetConcept T, typename... Args>
	static RID CreateAsset(const std::string& name, Args&&... args) {
		auto asset { MakeScope<T>(std::forward<Args>(args)...) };
		RID id = asset->GetId();
		s_Records[id] = { name, std::move(asset) };
		OnAssetCreated.Invoke(id, T::GetClassAssetType());

		return id;
	}

	static const std::unordered_map<RID, Record>& GetRecords() { return s_Records; }

	static Asset* GetAssetChecked(const RID& id);
	static std::string GetAssetName(const RID& id) { return s_Records[id].Name; }

	template <AssetConcept T>
	static T* GetAssetAs(RID id) {
		auto it { s_Records.find(id) };

		if (it == s_Records.end()) {
			ORG_INFO("Could not find {} Asset with runtime ID", id.ToString());
			return nullptr;
		}

		return static_cast<T*>(it->second.AssetReference.get());
	}

	static void SubscribeToAssetCreated(const AssetCreatedCallback& cb) {
		OnAssetCreated.AddListener(cb);
	}

private:
	struct Record {
		std::string Name {};
		Scope<Asset> AssetReference {};
	};

private:
	inline static std::unordered_map<RID, Record> s_Records {};

	inline static Action<void, RID, AssetType> OnAssetCreated {};
	inline static Action<void, RID, AssetType> OnAssetModified {};
	inline static Action<void, RID, AssetType> OnAssetRemoved {};
};
}
