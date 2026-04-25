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

struct MaterialPBRData {
	PBRParameters PBRParams;
	PBRTextures PBRTexs;
};

}
