#pragma once

#include "origo/assets/ShaderSource.h"
#include "origo/serialization/ISerializer.h"

namespace Origo::ShaderSourceSerializer {

void Serialize(Ref<ShaderSource>, ISerializer& backend);
Ref<ShaderSource> Deserialize(const nlohmann::json&);

}
