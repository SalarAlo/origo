#pragma once

#include "origo/assets/AssetManager.h"
#include "origo/assets/DefaultAssetCache.h"

#include "origo/renderer/PBRParameters.h"

namespace Origo {

enum class MaterialSourceType {
	File,
	Raw,
	ID
};

struct MaterialData {
	OptionalAssetHandle ShaderHandle { DefaultAssetCache::get_instance().get_shader() };

	PBRParameters PBRParams;
	PBRTextures PBRTexs;
};

}
