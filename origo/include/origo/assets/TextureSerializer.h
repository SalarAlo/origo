#pragma once

#include "origo/assets/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

class TextureSerializer : public AssetSerializer {
	bool ShouldSerialize(const Asset* asset) const override;
	void Serialize(const Asset* asset, ISerializer& backend) const override;
	Asset* Deserialize(ISerializer& backend) const override;
};

}
