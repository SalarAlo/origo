#pragma once

#include "origo/assets/serialization/AssetSerializer.h"

namespace Origo {

class MaterialColorSerializer : public AssetSerializer {
public:
	void serialize(const Asset* asset, ISerializer& backend) const override;
	void deserialize(ISerializer& backend, Asset& asset) const override;
};

}
