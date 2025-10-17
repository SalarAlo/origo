#include "origo/assets/TextureCache.h"

#include "origo/core/Logger.h"
#include "origo/renderer/Texture.h"

namespace Origo::TextureCache {
static std::unordered_map<std::string, Ref<Texture>> s_Cache;

Ref<Texture> Load(const std::string& path) {
	auto resultFind { s_Cache.find(path) };

	if (resultFind == s_Cache.cend()) {
		s_Cache[path] = MakeRef<Texture>(path);
	} else {
		ORG_CORE_INFO("[TextureCache] found cached value for shader with name {}", path);
	}

	return s_Cache[path];
}

void Clear() {
	s_Cache.clear();
}

}
