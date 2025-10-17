#pragma once

#include "origo/renderer/Shader.h"

namespace Origo::ShaderLibrary {

Ref<Shader> Load(const std::string& name);
void Register(const std::string& name, const std::string& vertShader, const std::string& fragShader);

}
