#pragma once

#include "origo/assets/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {
class ShaderSerializer : public AssetSerializer {
public:
	void Serialize(const Asset* asset, ISerializer& backend) const override;
	Asset* Deserialize(ISerializer& backend) const override;

private:
};
}
