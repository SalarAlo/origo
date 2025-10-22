#pragma once

#include "origo/assets/ShaderSource.h"

namespace Origo::ShaderSourceSerializer {

nlohmann::json Serialize(Ref<ShaderSource>);
Ref<ShaderSource> Deserialize(const nlohmann::json&);

}
