#pragma once
#include "Asset.h"
#include "nlohmann/json.hpp"
#include <memory>

namespace Origo {

class AssetSerializer {
public:
	virtual nlohmann::json Serialize(const Ref<Asset>& asset) const = 0;
	virtual Ref<Asset> Deserialize(const nlohmann::json& j) const = 0;
};

namespace AssetSerializationRegistry {
	void Register(AssetType type, Ref<AssetSerializer> serializer);
	Ref<AssetSerializer> Get(AssetType type);
}

#define REGISTER_ASSET_SERIALIZER(NAME)                                                                                         \
	namespace {                                                                                                             \
		auto x = []() {                                                                                                 \
			::Origo::AssetSerializationRegistry::Register(                                                          \
			    AssetType::NAME, std::dynamic_pointer_cast<AssetSerializer>(MakeRef<::Origo::NAME##Serializer>())); \
			return true;                                                                                            \
		}();                                                                                                            \
	}
}
