#include "nlohmann/json_fwd.hpp"
#include "origo/scene/Scene.h"
#include <nlohmann/json.hpp>

namespace Origo {

namespace SceneSerialization {
	void Serialize(const Scene& scene, ISerializer& backend);
	Scene Deserialize(nlohmann::json);
}

}
