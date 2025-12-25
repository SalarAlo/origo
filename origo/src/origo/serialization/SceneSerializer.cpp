#include "origo/scene/SceneSerialization.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {

void SceneSerialization::Serialize(const Scene& scene, ISerializer& backend) {
	backend.Write("scene_name", scene.m_Name);

	backend.BeginArray("entities");

	for (const auto& id : scene.m_Entities) {
		backend.BeginArrayElement();

		// TODO: serialize entity

		backend.EndArrayElement();
	}

	backend.EndArray();

	backend.SaveToFile();
}

}
