#pragma once

#include "origo/scene/Scene.h"

namespace Origo::SceneSerializer {

Scope<Scene> DeserializeFromFile(const std::filesystem::path& path);
void SerializeToFile(Scene& scene, const std::filesystem::path& out);

}
