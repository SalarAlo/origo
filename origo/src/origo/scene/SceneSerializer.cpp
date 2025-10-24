#include "origo/scene/SceneSerializer.h"
#include "origo/assets/AssetManager.h"
#include "origo/assets/AssetSerializer.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace Origo::SceneSerialization {

// TODO: is there really no way for components to be serialized antoher way. because if clients write them we're kinda cooked

void Serialize(std::string_view outPath, const Scene& scene) {
	using nlohmann::json;

	json sceneJson;
	sceneJson["scene_name"] = scene.m_Name;

	json entitiesJson = json::array();

	for (const auto& [id, entity] : scene.m_Entities) {
		json singleEntityJson;
		singleEntityJson["id"] = entity->GetId().ToString();
		singleEntityJson["name"] = entity->GetName();

		entitiesJson.push_back(singleEntityJson);
	}

	sceneJson["entities"] = entitiesJson;
	sceneJson["assets"] = ::Origo::AssetSerializationSystem::SaveAll();

	std::ofstream out(std::string("resources/scenes/") + std::string(outPath),
	    std::ios::out | std::ios::trunc);
	if (!out) {
		throw std::runtime_error("Failed to open output file: " + std::string(outPath));
	}

	out << sceneJson.dump(8);
	out.close();
}

}
