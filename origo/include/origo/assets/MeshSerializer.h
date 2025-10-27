#pragma once

#include "origo/assets/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"
namespace Origo {
class MeshSerializer : public AssetSerializer {
public:
	void Serialize(const Ref<Asset>& asset, ISerializer& backend) const override;
	Ref<Asset> Deserialize(ISerializer& backend) const override;
};

};
