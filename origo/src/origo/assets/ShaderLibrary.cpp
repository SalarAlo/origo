#include "origo/assets/ShaderLibrary.h"
#include "origo/renderer/Shader.h"
#include "origo/core/Logger.h"
#include "origo/renderer/ShaderSource.h"
#include <unordered_map>

namespace Origo::ShaderLibrary {
static std::unordered_map<std::string, Ref<Shader>> s_Cache;

Ref<Shader> Load(const std::string& name) {
	auto resultFind { s_Cache.find(name) };

	if (resultFind == s_Cache.cend()) {
		s_Cache[name] = MakeRef<Shader>(MakeRef<ShaderSourceFile>(name));
	} else {
		ORG_CORE_INFO("[ShaderCache] found cached value for shader with name {}", name);
	}

	return s_Cache[name];
}

void Register(const std::string& name, const std::string& vertShader, const std::string& fragShader) {
	s_Cache[name] = MakeRef<Shader>(MakeRef<ShaderSourceRaw>(vertShader, fragShader));
}

}
