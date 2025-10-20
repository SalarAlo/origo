#include "origo/assets/AssetSerializer.h"
#include "origo/assets/ShaderSerializer.h"
#include "magic_enum/magic_enum.hpp"

namespace Origo::AssetSerializationRegistry {

static std::unordered_map<AssetType, Ref<AssetSerializer>>& GetRegistry() {
	static std::unordered_map<AssetType, Ref<AssetSerializer>> registry;
	return registry;
}

void Register(AssetType type, Ref<AssetSerializer> serializer) {
	auto& reg = GetRegistry();
	std::cout << "Registering serializer for type " << magic_enum::enum_name(type) << std::endl;
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
}

}
