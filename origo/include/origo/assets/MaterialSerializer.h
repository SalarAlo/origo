#pragma once

#include "origo/assets/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class MaterialSerializer : public AssetSerializer {
public:
	void Serialize(const Asset* asset, ISerializer& backend) const override;
	Scope<Asset> Deserialize(ISerializer& backend) const override;

private:
};

}
