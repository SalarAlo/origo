#include "AssetFactory.h"

#include "origo/assets/material/MaterialPBR.h"
#include "origo/assets/Model.h"
#include "origo/assets/SceneAsset.h"
#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"
#include "origo/assets/Texture2D.h"

namespace Origo {
Scope<Asset> AssetFactory::allocate_hollow_asset(AssetType type) {
	switch (type) {
	case AssetType::Shader:
		return make_scope<Shader>();
	case AssetType::Texture2D:
		return make_scope<Texture2D>();
	case AssetType::MaterialPBR:
		return make_scope<MaterialPBR>();
	case AssetType::Script:
		return make_scope<Script>();
	case AssetType::Model:
		return make_scope<Model>();
	case AssetType::Scene:
		return make_scope<SceneAsset>();
	default:
		ORG_ERROR("AssetFactory: Unknown asset type");
		return nullptr;
	}
}

}
