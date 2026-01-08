#pragma once

#include "origo/assets/AssetManagerFast.h"
namespace Origo {

class SkyboxDefaults {
public:
	static AssetHandle GetCubemap();
	static AssetHandle GetShader();
	static AssetHandle GetMaterial();
};

}
