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
	Scene(std::string_view = "SampleScene", float ar = 1.0);

	Ref<Entity> CreateEntity(std::string_view name);
	void Render();

	Camera& GetCamera() {
		return m_Camera;
	}

	const std::string& GetName() const {
		return m_Name;
	}

	template <ComponentConcept T, typename... Args>
	Ref<T> AddComponent(const Ref<Entity>& entity, Args&&... args) {
		return m_ComponentManager.AddComponent<T>(entity, std::forward<Args>(args)...);
	}

private:
	ComponentManager m_ComponentManager;
	std::string m_Name;
	std::unordered_map<UUID, Ref<Entity>> m_Entities;
	Camera m_Camera {};
};
}
