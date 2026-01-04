#pragma once
#include "Asset.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class AssetSerializer {
public:
	virtual ~AssetSerializer() = default;
	virtual void Serialize(const Asset* asset, ISerializer& backend) const = 0;
	virtual void Deserialize(ISerializer& backend, Asset& asset) const = 0;
};

namespace AssetSerializationSystem {
	void Register(AssetType type, AssetSerializer* serializer);
	AssetSerializer* Get(AssetType type);

	void RegisterAllAssetSerializers();
	void Cleanup();

}
}
