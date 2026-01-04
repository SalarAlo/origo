#pragma once

#include "origo/assets/Asset.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class TextureSerializer : public AssetSerializer {
	void Serialize(const Asset* asset, ISerializer& backend) const override;
	void Deserialize(ISerializer& backend, Asset& asset) const override;
};

}
