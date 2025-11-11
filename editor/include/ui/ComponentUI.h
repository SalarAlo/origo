#pragma once

#include "glm/fwd.hpp"

namespace ComponentUI {
void DrawVec3Control(std::string_view label, glm::vec3& values, float speed = 0.1f);
}
