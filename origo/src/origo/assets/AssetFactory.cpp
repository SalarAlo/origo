#include "AssetFactory.h"

#include "origo/assets/Material2D.h"
#include "origo/assets/Model.h"
#include "origo/assets/SceneAsset.h"
#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"

namespace Origo {
Scope<Asset> AssetFactory::allocate_hollow_asset(AssetType type) {
	switch (type) {
	case AssetType::Shader:
		return MakeScope<Shader>();
	case AssetType::Texture2D:
		return MakeScope<Texture2D>();
	case AssetType::Material2D:
		return MakeScope<Material2D>();
	case AssetType::Script:
		return MakeScope<Script>();
	case AssetType::Model:
		return MakeScope<Model>();
	case AssetType::Scene:
		return MakeScope<SceneAsset>();
	default:
		ORG_ERROR("AssetFactory: Unknown asset type");
		return nullptr;
	}
}

}
