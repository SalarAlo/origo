#include "origo/core/Init.h"
#include "origo/assets/importers/AssetPipeline.h"

namespace Origo {
void Init() {
	AssetPipeline::RunInitialImport();
}
}
