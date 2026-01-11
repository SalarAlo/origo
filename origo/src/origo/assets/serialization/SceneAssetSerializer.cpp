#include "origo/assets/serialization/SceneAssetSerializer.h"
#include "origo/assets/SceneAsset.h"
#include "origo/core/Logger.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void SceneAssetSerializer::Serialize(const Asset* asset, ISerializer& backend) const {
	ORG_INFO("Seriliazing an asset of type model");

	auto model { static_cast<const SceneAsset*>(asset) };

	auto path { model->GetPath() };
	backend.Write("path", path->c_str());
}

void SceneAssetSerializer::Deserialize(ISerializer& backend, Asset& asset) const {
	std::string path {};

	if (!backend.TryRead("path", path)) {
		ORG_CORE_WARN("SceneSerializer::Deserialize: No Path provided");
	}

	auto& model { static_cast<SceneAsset&>(asset) };

	model.SetPath(path);
}

}
