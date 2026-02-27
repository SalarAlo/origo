#pragma once

#include "origo/assets/serialization/AssetSerializer.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {

class MaterialSerializer : public AssetSerializer {
public:
	void serialize(const Asset* asset, ISerializer& backend) const override;
	void deserialize(ISerializer& backend, Asset& asset) const override;

private:
};

}
