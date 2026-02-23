#pragma once

#include "origo/assets/AssetManager.h"

namespace Origo {

class PrimitiveShapeCache : public Singleton<PrimitiveShapeCache> {
public:
	void CreateAllPrimitiveShapes();

	AssetHandle GetCubeMesh();
	AssetHandle GetSphereMesh();
	AssetHandle GetQuadMesh();
	AssetHandle GetConeMesh();
};

}
