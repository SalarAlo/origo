#pragma once

#include "origo/assets/AssetDatabase.h"
#include "origo/assets/Metadata.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/assets/Material.h"
#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"
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

		AssetMetadata meta {};
		meta.Name = name;
		meta.ID = id;
		meta.Type = type;
		meta.Origin = AssetOrigin::Generated;

		AssetDatabase::RegisterMetadata(meta);

		auto asset { MakeScope<T>(std::forward<Args>(args)...) };

		return AssetManagerFast::GetInstance().Register(std::move(asset), id);
	}

	static Scope<Asset> AllocateHollowAsset(AssetType type) {
		switch (type) {
		case AssetType::Shader:
			return MakeScope<Shader>();
		case AssetType::Texture2D:
			return MakeScope<Texture2D>();
		case AssetType::Material:
			return MakeScope<Material2D>();
		case AssetType::Script:
			return MakeScope<Script>();
		default:
			ORG_ERROR("AssetFactory: Unknown asset type");
			return nullptr;
		}
	}
};

}
