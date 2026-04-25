#pragma once
#include "origo/assets/Asset.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

class AssetSerializer {
public:
	virtual ~AssetSerializer() = default;
	virtual void serialize(const Asset* asset, ISerializer& backend) const = 0;
	virtual void deserialize(ISerializer& backend, Asset& asset) const = 0;
};

namespace AssetSerializationSystem {
	void register_serializer(AssetType type, AssetSerializer* serializer);
	AssetSerializer* get(AssetType type);

	void register_all_asset_serializers();
	void cleanup();

}
}
