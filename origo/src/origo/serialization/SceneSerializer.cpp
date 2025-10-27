#include "origo/scene/SceneSerialization.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetSerializer.h"
#include "origo/serialization/ISerializer.h"

namespace Origo::SceneSerialization {

void Serialize(const Scene& scene, ISerializer& backend) {
	backend.Write("scene_name", scene.m_Name);

	backend.BeginArray("entities");
	for (const auto& [id, entity] : scene.m_Entities) {
		backend.BeginArrayElement();

		backend.Write("id", entity->GetId().ToString());
		backend.Write("name", entity->GetName());

		backend.EndArrayElement();
	}
	backend.EndArray();

	AssetSerializationSystem::SaveAll(backend);

	backend.WriteFile();
}

}
