#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <spdlog/spdlog.h>

#include <typeindex>
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <string_view>
#include <cstdlib>
#include <cstddef>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <stdexcept>

#include <exception>
#include <stdexcept>

#include "origo/include/origo/core/Typedefs.h"
#include "origo/include/origo/renderer/GlDebug.h"

#include "magic_enum/magic_enum.hpp"
