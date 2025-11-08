#pragma once

#include "origo/Camera.h"
#include "origo/core/Identifiable.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/ComponentManager.h"
#include "origo/serialization/ISerializer.h"
#include <unordered_map>

namespace Origo {
class Scene;

namespace SceneSerialization {
	void Serialize(const Scene& scene, ISerializer& backend);
}

class Scene {
	friend void SceneSerialization::Serialize(const Scene& scene, ISerializer& backend);

public:
	Scene(std::string_view = "SampleScene", float ar = 1.0);
	~Scene();

	Entity* CreateEntity(std::string_view name);

	const std::string& GetName() const {
		return m_Name;
	}

	Camera* GetMainCamera() { return m_MainCamera; }

#pragma region FORWARDING
	template <ComponentConcept T, typename... Args>
	T* AddComponent(Entity* entity, Args&&... args) {
		return m_ComponentManager.AddComponent<T>(entity, std::forward<Args>(args)...);
	}

	template <ComponentConcept T, typename... Args>
	T* GetComponent(const UUID& entity) const {
		return m_ComponentManager.GetComponent<T>(entity);
	}

	template <ComponentConcept T>
	std::vector<T*> GetAllComponentsOfType() const {
		return m_ComponentManager.GetAllComponentsOfType<T>();
	}
#pragma endregion

private:
	ComponentManager m_ComponentManager;
	Camera* m_MainCamera;
	std::string m_Name;
	std::unordered_map<UUID, Entity*> m_Entities;
};
}
