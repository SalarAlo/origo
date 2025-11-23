#pragma once

#include "origo/assets/Asset.h"
#include "origo/renderer/MeshRange.h"
namespace Origo {

struct MeshAlternative : public Asset {
	int LayoutId { -1 };
	int HeapId { -1 };
	MeshRange Range;

	AssetType GetAssetType() const override {
		return AssetType::MeshAlternative;
	}
};

}
