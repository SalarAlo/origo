#pragma once

#include "origo/assets/AssetSerializer.h"
namespace Origo {
class MeshSerializer : public AssetSerializer {
public:
	nlohmann::json Serialize(const Ref<Asset>& asset) const override;
	Ref<Asset> Deserialize(const nlohmann::json& j) const override;
};

};
