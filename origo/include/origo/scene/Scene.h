#pragma once

#include "origo/Camera.h"
#include "origo/core/Identifiable.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/ComponentManager.h"
#include <unordered_map>
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

	ComponentManager m_ComponentManager;

private:
	std::string m_Name;
	std::unordered_map<UUID, Ref<Entity>> m_Entities;
	Camera m_Camera {};
};
}
