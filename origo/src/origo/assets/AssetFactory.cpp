#include "AssetFactory.h"

namespace Origo {
Scope<Asset> AssetFactory::AllocateHollowAsset(AssetType type) {
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
	default:
		ORG_ERROR("AssetFactory: Unknown asset type");
		return nullptr;
	}
}

}
