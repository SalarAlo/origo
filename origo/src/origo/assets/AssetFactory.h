#pragma once

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/Metadata.h"
#include "origo/assets/Script.h"

#include "origo/core/UUID.h"

namespace Origo {

class AssetFactory {
public:
	using AssetCreatedCallback = std::function<void(UUID, AssetType)>;

public:
	template <typename T, typename... Args>
	static AssetHandle CreateRuntimeAsset(const std::string& name, Args&&... args) {
		AssetMetadata meta {};
		meta.Name = name;
		meta.ID = UUID::Generate();
		meta.Type = T::GetClassAssetType();
		meta.Origin = AssetOrigin::Runtime;

		AssetDatabase::RegisterMetadata(meta);

		auto asset = MakeScope<T>(std::forward<Args>(args)...);
		return AssetManager::GetInstance().Register(std::move(asset), meta.ID);
	}

	template <typename T, typename... Args>
	static AssetHandle CreateSyntheticAsset(const std::string& name, const UUID& uuid, Args&&... args) {
		AssetMetadata meta {};
		meta.Name = name;
		meta.ID = uuid;
		meta.Type = T::GetClassAssetType();
		meta.Origin = AssetOrigin::Synthetic;

		AssetDatabase::RegisterMetadata(meta);

		auto asset = MakeScope<T>(std::forward<Args>(args)...);
		return AssetManager::GetInstance().Register(std::move(asset), uuid);
	}

	static AssetHandle CreateImportedAsset(const AssetMetadata& meta, Scope<Asset>&& asset) {
		AssetDatabase::RegisterMetadata(meta);

		return AssetManager::GetInstance().Register(
		    std::move(asset),
		    meta.ID,
		    meta.SourcePath);
	}

	static Scope<Asset> AllocateHollowAsset(AssetType type);
	template <AssetConcept T>
	static Scope<T> AllocateHollowAsset() {
		return MakeScope<T>();
	}
};

}
