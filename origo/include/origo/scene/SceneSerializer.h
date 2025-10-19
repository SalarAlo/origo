#include "nlohmann/json_fwd.hpp"
#include "origo/scene/Scene.h"
#include <nlohmann/json.hpp>

namespace Origo::SceneSerialization {

void Serialize(std::string_view outPath, const Scene& scene);
Scene Deserialize(nlohmann::json);

}
