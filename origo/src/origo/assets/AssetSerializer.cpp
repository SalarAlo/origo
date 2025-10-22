#include "origo/assets/AssetSerializer.h"
#include "origo/assets/MaterialSerializer.h"
#include "origo/assets/MeshSerializer.h"
#include "origo/assets/ModelSerializer.h"
#include "origo/assets/ShaderSerializer.h"
#include "origo/assets/TextureSerializer.h"

namespace Origo::AssetSerializationRegistry {

static std::unordered_map<AssetType, Ref<AssetSerializer>>& GetRegistry() {
	static std::unordered_map<AssetType, Ref<AssetSerializer>> registry;
	return registry;
}

void Register(AssetType type, Ref<AssetSerializer> serializer) {
	auto& reg = GetRegistry();
	reg[type] = std::move(serializer);
}

Ref<AssetSerializer> Get(AssetType type) {
	auto& reg = GetRegistry();
	auto it = reg.find(type);
	if (it != reg.end())
		return it->second;
	return nullptr;
}

void RegisterAllAssetSerializers() {
	Register(AssetType::Shader, MakeRef<ShaderSerializer>());
	Register(AssetType::Texture, MakeRef<TextureSerializer>());
	Register(AssetType::Material, MakeRef<MaterialSerializer>());
	Register(AssetType::Mesh, MakeRef<MeshSerializer>());
	Register(AssetType::Model, MakeRef<ModelSerializer>());
}

}
