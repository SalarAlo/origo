#pragma once

#include "origo/scene/Scene.h"

namespace Origo::SceneSerializer {

void SerializeToFile(Scene& scene, const std::filesystem::path& out);

}
