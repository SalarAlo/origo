#pragma once

#include <glm/glm.hpp>

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
	glm::vec3 BoundsMin { -0.5f, -0.5f, -0.5f };
	glm::vec3 BoundsMax { 0.5f, 0.5f, 0.5f };

	AssetType get_asset_type() const override {
		return AssetType::Mesh;
	}

	static AssetType get_class_asset_type() { return AssetType::Mesh; }

	void set_bounds(const glm::vec3& min, const glm::vec3& max) {
		BoundsMin = min;
		BoundsMax = max;
	}
};

}
