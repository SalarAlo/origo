#pragma once

#include "origo/assets/AssetManager.h"

namespace Origo {

class PrimitiveShapeCache : public Singleton<PrimitiveShapeCache> {
public:
	AssetHandle GetCubeMesh();
	AssetHandle GetSphereMesh();
	AssetHandle GetQuadMesh();
	AssetHandle GetConeMesh();
};

}
