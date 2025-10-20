#pragma once
#include "Asset.h"
#include "nlohmann/json_fwd.hpp"

namespace Origo {

class AssetSerializer {
public:
	virtual nlohmann::json Serialize(const Ref<Asset>& asset) const = 0;
	virtual Ref<Asset> Deserialize(const nlohmann::json& j) const = 0;
};

namespace AssetSerializationRegistry {
	void Register(AssetType type, Ref<AssetSerializer> serializer);
	Ref<AssetSerializer> Get(AssetType type);

	void RegisterAllAssetSerializers();
}

}
