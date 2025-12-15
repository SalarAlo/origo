#pragma once

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/AssetDescriptor.h"
#include "origo/assets/AssetEvents.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Material.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture.h"
#include "origo/core/UUID.h"

namespace Origo {

class AssetFactory {
public:
	using AssetCreatedCallback = std::function<void(UUID, AssetType)>;

public:
	template <AssetConcept T, typename... Args>
	static AssetHandle CreateAsset(const std::string& name, Args&&... args) {
		UUID id { UUID::Generate() };
		AssetType type = T::GetClassAssetType();

		Metadata meta {};
		meta.Name = name;
		meta.Id = id;
		meta.Type = type;
		meta.Origin = AssetOrigin::Generated;

		AssetEvents::OnAssetCreated.Invoke(meta);

		AssetDatabase::RegisterMetadata(meta);

		auto asset { MakeScope<T>(std::forward<Args>(args)...) };

		return AssetManagerFast::GetInstance().Register(std::move(asset), id);
	}

	static Scope<Asset> Create(AssetType type) {
		switch (type) {
		case AssetType::Shader:
			return MakeScope<Shader>();
		case AssetType::Texture:
			return MakeScope<Texture>();
		case AssetType::Material:
			return MakeScope<Material>();
		default:
			ORG_ERROR("AssetFactory: Unknown asset type");
			return nullptr;
		}
	}
};

}
