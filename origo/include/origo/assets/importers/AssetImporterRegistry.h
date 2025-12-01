#pragma once

#include "origo/assets/importers/IAssetImporter.h"

namespace Origo {
class AssetImporterRegistry {
public:
private:
	inline static std::vector<Scope<IAssetImporter>> s_Importers {};
};

}
