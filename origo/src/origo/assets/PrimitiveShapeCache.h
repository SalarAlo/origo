#pragma once

#include "origo/assets/AssetManager.h"

namespace Origo {

class PrimitiveShapeCache {
public:
	static PrimitiveShapeCache& GetInstance();

	AssetHandle GetCubeMesh();
	AssetHandle GetSphereMesh();
	AssetHandle GetQuadMesh();

private:
	PrimitiveShapeCache() = default;

	PrimitiveShapeCache(const PrimitiveShapeCache&) = default;
	PrimitiveShapeCache& operator=(const PrimitiveShapeCache&) = default;

	PrimitiveShapeCache(PrimitiveShapeCache&&) = default;
	PrimitiveShapeCache& operator=(PrimitiveShapeCache&&) = default;
};

}
