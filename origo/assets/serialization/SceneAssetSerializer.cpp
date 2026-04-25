#include "origo/assets/serialization/SceneAssetSerializer.h"

#include "origo/assets/SceneAsset.h"

#include "origo/core/PathContext.h"

#include "origo/core/Logger.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

void SceneAssetSerializer::serialize(const Asset* asset, ISerializer& backend) const {
	ORG_CORE_TRACE("Seriliazing an asset of type model");

	auto model { static_cast<const SceneAsset*>(asset) };

	auto path { model->get_path() };
	backend.write("path", path ? PathContextService::get_instance().serialize_path(*path).string() : std::string());
}

void SceneAssetSerializer::deserialize(ISerializer& backend, Asset& asset) const {
	std::string path {};

	if (!backend.try_read("path", path)) {
		ORG_CORE_WARN("SceneSerializer::Deserialize: No Path provided");
	}

	auto& model { static_cast<SceneAsset&>(asset) };

	model.set_path(PathContextService::get_instance().resolve_serialized_path(path));
}

}
