#pragma once

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetMetadata.h"
#include "origo/assets/AssetEvents.h"
#include "origo/core/RID.h"

namespace Origo {

class AssetFactory {
public:
	using AssetCreatedCallback = std::function<void(RID, AssetType)>;

public:
	template <AssetConcept T, typename... Args>
	static RID CreateAsset(const std::string& name, Args&&... args) {
		UUID uuid {};
		AssetType type = T::GetClassAssetType();

		AssetMetadata meta {};
		meta.Name = name;
		meta.Id = uuid;
		meta.Type = type;
		meta.Origin = AssetOrigin::Generated;

		AssetEvents::OnAssetCreated.Invoke(meta);

		AssetDatabase::RegisterMetadata(meta);

		auto asset { MakeScope<T>(std::forward<Args>(args)...) };
		RID id = asset->GetId();

		AssetManager::Register(std::move(asset), &uuid);

		return id;
	}
};

}
