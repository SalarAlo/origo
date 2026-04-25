#pragma once

#include "origo/scene/Scene.h"

namespace Origo::SceneSerializer {

Scope<Scene> deserialize_from_file(const std::filesystem::path& path);
void serialize_to_file(Scene& scene, const std::filesystem::path& out);

}
