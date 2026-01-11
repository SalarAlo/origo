#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/assets/serialization/AssetSerializer.h"
#include "origo/assets/serialization/MaterialSerializer.h"
#include "origo/assets/serialization/MeshSerializer.h"
#include "origo/assets/serialization/ModelSerializer.h"
#include "origo/assets/serialization/SceneAssetSerializer.h"
#include "origo/assets/serialization/ScriptSerializer.h"
#include "origo/assets/serialization/ShaderSerializer.h"
#include "origo/assets/serialization/TextureSerializer.h"

namespace Origo::AssetSerializationSystem {

static std::unordered_map<AssetType, AssetSerializer*>& GetRegistry() {
	static std::unordered_map<AssetType, AssetSerializer*> registry;
	return registry;
}

void Register(AssetType type, AssetSerializer* serializer) {
	auto& reg = GetRegistry();
	reg[type] = serializer;
}

void Cleanup() {
	for (const auto& kv : GetRegistry()) {
		delete kv.second;
	}
}

AssetSerializer* Get(AssetType type) {
	auto& reg = GetRegistry();
	auto it = reg.find(type);
	if (it != reg.end())
		return it->second;
	return nullptr;
}

void RegisterAllAssetSerializers() {
	ORG_CORE_TRACE("Registering all default asset serializers");

	Register(AssetType::Texture2D, new TextureSerializer());
	Register(AssetType::Material2D, new MaterialSerializer());
	Register(AssetType::Mesh, new MeshSerializer());
	Register(AssetType::Model, new ModelSerializer());
	Register(AssetType::Shader, new ShaderSerializer());
	Register(AssetType::Script, new ScriptSerializer());
	Register(AssetType::Scene, new SceneAssetSerializer());
}

}
