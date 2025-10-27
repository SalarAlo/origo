#pragma once

#include "origo/assets/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"
namespace Origo {

class ModelSerializer : public AssetSerializer {
public:
	void Serialize(const Ref<Asset>& asset, ISerializer& backend) const override;
	Ref<Asset> Deserialize(ISerializer& backend) const override;

private:
};

}
