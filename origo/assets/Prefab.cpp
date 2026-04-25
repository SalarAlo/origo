#include "Prefab.h"

#include "origo/assets/Asset.h"

#include "origo/assets/serialization/SceneEntitySerializationManager.h"

namespace Origo {

Prefab::Prefab(const RID& e, const Scene* scene) {
	SceneEntitySerializationManager serializer {};
	serializer.serialize_entity_data(*scene, e, backend);
}

AssetType Prefab::get_asset_type() const {
	return AssetType::Prefab;
};

}
