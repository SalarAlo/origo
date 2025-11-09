#pragma once

#include "origo/assets/ShaderSource.h"
#include "origo/serialization/ISerializer.h"

namespace Origo::ShaderSourceSerializer {

void Serialize(ShaderSource*, ISerializer& backend);
ShaderSource* Deserialize(const nlohmann::json&);

}
