#pragma once

#include "nlohmann/json.hpp"
#include "origo/assets/Shader.h"
#include "origo/assets/ShaderSource.h"

namespace Origo::ShaderSourceSerializer {

nlohmann::json Serialize(Ref<ShaderSource>);
Ref<ShaderSource> Deserialize(const nlohmann::json&);

}
