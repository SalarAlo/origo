#pragma once

#include "origo/assets/AssetManagerFast.h"

namespace Origo {

class PrimitiveShapeCache {
public:
	static PrimitiveShapeCache& GetInstance();

	AssetHandle GetCubeMesh();
	AssetHandle GetSphereMesh();
	AssetHandle GetQuadMesh();

	PrimitiveShapeCache(const PrimitiveShapeCache&) = delete;
	void operator=(const PrimitiveShapeCache&) = delete;

	PrimitiveShapeCache(PrimitiveShapeCache&&) = delete;
	void operator=(PrimitiveShapeCache&&) = delete;

private:
	PrimitiveShapeCache() = default;
};

}
