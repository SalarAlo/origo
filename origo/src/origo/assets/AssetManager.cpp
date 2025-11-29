#include "origo/assets/AssetManager.h"
#include "origo/assets/Asset.h"
#include <unordered_map>

namespace Origo {

Asset* AssetManager::GetAssetChecked(const RID& id) {
	auto it { s_Records.find(id) };
	return it != s_Records.end() ? it->second.AssetReference.get() : nullptr;
}

}
