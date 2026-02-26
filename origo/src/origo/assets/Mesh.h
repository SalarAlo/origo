#pragma once

#include "origo/assets/Asset.h"

#include "origo/renderer/MeshRange.h"

namespace Origo {

struct Mesh : public Asset {
	Mesh(int layoutId, int heapId, const MeshRange& range)
	    : LayoutId(layoutId)
	    , HeapId(heapId)
	    , Range(range) { };

	int LayoutId { -1 };
	int HeapId { -1 };

	MeshRange Range;

	AssetType get_asset_type() const override {
		return AssetType::Mesh;
	}

	static AssetType get_class_asset_type() { return AssetType::Mesh; }
};

}
