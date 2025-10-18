#pragma once

#include "origo/Camera.h"
#include "origo/scene/Entity.hpp"
#include <queue>
#

namespace Origo {
class Scene;

namespace SceneSerialization {
	void Serialize(std::string_view outPath, const Scene& scene);
}

class Scene {
	friend void SceneSerialization::Serialize(std::string_view outPath, const Scene& scene);

public:
	Scene(std::string_view = "SampleScene");
	Ref<Entity> CreateEntity(std::string_view name);
	void Render();
	Camera& GetCamera();
	const std::string& GetName() const;

private:
	std::string m_Name;

	std::unordered_map<EntityId, Ref<Entity>, EntityIdHash> m_Entities;
	Camera m_Camera {};

	std::queue<int> m_FreeIndices;
	std::vector<int> m_Generation {};
};
}
