#pragma once

#include "origo/assets/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class MaterialSerializer : public AssetSerializer {
public:
	void Serialize(const Asset* asset, ISerializer& backend) const override;
	void Deserialize(ISerializer& backend, Asset& asset) const override;

private:
};

}
