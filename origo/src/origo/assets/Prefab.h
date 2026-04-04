#pragma once

#include "origo/assets/Asset.h"

#include "origo/core/RID.h"

#include "origo/scene/Scene.h"

#include "origo/serialization/JsonSerializer.h"

namespace Origo {
class Prefab : public Asset {
public:
	Prefab() = default;
	Prefab(const RID& e, const Scene* scene);

	virtual AssetType get_asset_type() const;
	static AssetType get_class_asset_type() { return AssetType::Prefab; }

	JsonSerializer backend {};
};

}
