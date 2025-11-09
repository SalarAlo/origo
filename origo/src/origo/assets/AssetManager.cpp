#include "origo/assets/AssetManager.h"
#include <unordered_map>

namespace Origo {

Asset* AssetManager::GetAsset(UUID id) {
	auto it { s_Records.find(id) };
	return it != s_Records.end() ? it->second.AssetReference.get() : nullptr;
}

}
