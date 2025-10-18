#include "origo/scene/SceneSerializer.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace Origo::SceneSerialization {

void Serialize(std::string_view outPath, const Scene& scene) {
	using nlohmann::json;

	json sceneJson;
	sceneJson["scene_name"] = scene.m_Name;

	json entitiesJson = json::array();
	for (const auto& [id, entity] : scene.m_Entities) {
		json singleEntityJson;
		singleEntityJson["id"] = EntityIdHash {}(id);
		singleEntityJson["name"] = entity->GetName();

		entitiesJson.push_back(singleEntityJson);
	}

	sceneJson["entities"] = entitiesJson;

	// --- Write to readable JSON file ---
	std::ofstream out(std::string("resources/scenes/") + std::string(outPath),
	    std::ios::out | std::ios::trunc);
	if (!out) {
		throw std::runtime_error("Failed to open output file: " + std::string(outPath));
	}

	out << sceneJson.dump(4); // 4-space indentation for readability
	out.close();
}

} // namespace Origo::SceneSerialization
