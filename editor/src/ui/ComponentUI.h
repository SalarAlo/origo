#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "glm/fwd.hpp"

#include "origo/assets/Asset.h"
#include "origo/assets/AssetManager.h"

namespace ComponentUI {

void DrawVec3Control(std::string_view label, glm::vec3& values, float speed = 0.1f);

void DrawFloatControl(std::string_view label, float& value, float speed = 0.1f);
void DrawFloatControl(std::string_view label, float& value, float speed, const char* format);

void DrawIntControl(std::string_view label, int& value, float speed = 0.1f);

void DrawStringControl(std::string_view label, std::string& value);

void DrawBoolControl(std::string_view label, bool& value);

void DrawAssetControl(
    std::string_view label,
    Origo::OptionalAssetHandle& handle,
    std::optional<Origo::AssetType> assetValidationType = std::nullopt);

void DrawColorControl(std::string_view label, glm::vec3& value);

}
