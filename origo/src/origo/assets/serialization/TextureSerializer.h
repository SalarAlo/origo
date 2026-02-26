#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/serialization/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class TextureSerializer : public AssetSerializer {
	void serialize(const Asset* asset, ISerializer& backend) const override;
	void deserialize(ISerializer& backend, Asset& asset) const override;
};

}
