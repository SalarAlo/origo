#pragma once

#include "glm/fwd.hpp"

namespace ComponentUI {
void DrawVec3Control(std::string_view label, glm::vec3& values, float speed = 0.1f);
void DrawFloatControl(std::string_view label, int& value, float speed = 0.1f);
void DrawIntControl(std::string_view label, int& value, float speed = 0.1f);
void DrawStringControl(std::string_view label, std::string& value);
}
