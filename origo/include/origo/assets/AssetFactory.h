#pragma once

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetDescriptor.h"
#include "origo/assets/AssetEvents.h"
#include "origo/core/UUID.h"

namespace Origo {

class AssetFactory {
public:
	using AssetCreatedCallback = std::function<void(UUID, AssetType)>;

public:
	template <AssetConcept T, typename... Args>
	static UUID CreateAsset(const std::string& name, Args&&... args) {
		UUID id { UUID::Generate() };
		AssetType type = T::GetClassAssetType();

		AssetDescriptor meta {};
		meta.Name = name;
		meta.Id = id;
		meta.Type = type;
		meta.Origin = AssetOrigin::Generated;

		AssetEvents::OnAssetCreated.Invoke(meta);

		AssetDatabase::RegisterMetadata(meta);

		auto asset { MakeScope<T>(std::forward<Args>(args)...) };

		AssetManager::Register(std::move(asset), id);

		return id;
	}
};

}
