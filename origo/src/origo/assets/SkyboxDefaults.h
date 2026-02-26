#pragma once

#include "origo/assets/AssetManager.h"
namespace Origo {

class SkyboxDefaults {
public:
	static AssetHandle get_cubemap();
	static AssetHandle get_shader();
	static AssetHandle get_material();
};

}
