#include "origo/core/Init.h"
#include "origo/assets/AssetSerializer.h"

namespace Origo {
void Init() {
	AssetSerializationRegistry::RegisterAllAssetSerializers();
}
}
