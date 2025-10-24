#pragma once
#include "Asset.h"
#include "nlohmann/json_fwd.hpp"

namespace Origo {

class AssetSerializer {
public:
	virtual nlohmann::json Serialize(const Ref<Asset>& asset) const = 0;
	virtual Ref<Asset> Deserialize(const nlohmann::json& j) const = 0;
};

namespace AssetSerializationSystem {
	void Register(AssetType type, Ref<AssetSerializer> serializer);
	Ref<AssetSerializer> Get(AssetType type);

	void RegisterAllAssetSerializers();

	nlohmann::json SaveAll();
	nlohmann::json LoadAll();
}

// idk if i wanna use this because of... SIOF
#define REGISTER_SERIALIZER(NAME)                                                               \
	namespace {                                                                             \
		const bool s_##NAME##_serializer_registered = [] {                              \
			Origo::AssetSerializationSystem::Register(                              \
			    Origo::AssetType::NAME, Origo::MakeRef<Origo::NAME##Serializer>()); \
			return true;                                                            \
		}();                                                                            \
	}

}
