#pragma once

#include "origo/assets/AssetManager.h"

#include "origo/utils/Singleton.h"

namespace Origo {

class SkyboxDefaults : public Singleton<SkyboxDefaults> {
public:
	AssetHandle get_cubemap();
	AssetHandle get_shader();
	AssetHandle get_material();
};

}
