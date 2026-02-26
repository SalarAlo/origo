#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/assets/serialization/MaterialSerializer.h"
#include "origo/assets/serialization/MeshSerializer.h"
#include "origo/assets/serialization/ModelSerializer.h"
#include "origo/assets/serialization/SceneAssetSerializer.h"
#include "origo/assets/serialization/ScriptSerializer.h"
#include "origo/assets/serialization/ShaderSerializer.h"
#include "origo/assets/serialization/TextureSerializer.h"

namespace Origo::AssetSerializationSystem {

static std::unordered_map<AssetType, AssetSerializer*>& get_registry() {
	static std::unordered_map<AssetType, AssetSerializer*> registry;
	return registry;
}

void register_serializer(AssetType type, AssetSerializer* serializer) {
	auto& reg = get_registry();
	reg[type] = serializer;
}

void cleanup() {
	for (const auto& kv : get_registry()) {
		delete kv.second;
	}
}

AssetSerializer* get(AssetType type) {
	auto& reg = get_registry();
	auto it = reg.find(type);
	if (it != reg.end())
		return it->second;
	return nullptr;
}

void register_all_asset_serializers() {
	ORG_CORE_TRACE("Registering all default asset serializers");

	register_serializer(AssetType::Texture2D, new TextureSerializer());
	register_serializer(AssetType::Material2D, new MaterialSerializer());
	register_serializer(AssetType::Mesh, new MeshSerializer());
	register_serializer(AssetType::Model, new ModelSerializer());
	register_serializer(AssetType::Shader, new ShaderSerializer());
	register_serializer(AssetType::Script, new ScriptSerializer());
	register_serializer(AssetType::Scene, new SceneAssetSerializer());
}

}
