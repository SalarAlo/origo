#pragma once

#include <variant>

#include "origo/assets/Script.h"
#include "origo/assets/Shader.h"

namespace OrigoEditor {

using TextEditableAsset = std::variant<Origo::Script*, Origo::Shader*>;

}
