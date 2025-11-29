#include "origo/assets/AssetSerializer.h"

#include "origo/assets/AssetSerializer.h"

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

}
