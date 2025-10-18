#include "origo/scene/Scene.h"
#include <nlohmann/json.hpp>

namespace Origo::SceneSerialization {
void Serialize(std::string_view outPath, const Scene& scene);
Scene Deserialize(std::string_view path);
}
