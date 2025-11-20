#pragma once

#include "origo/Camera.h"
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
	template <ComponentType T, typename... Args>
	T* AddComponent(Entity* entity, Args&&... args) {
		return m_ComponentManager.AddComponent<T>(entity, std::forward<Args>(args)...);
	}

	template <ComponentType T, typename... Args>
	T* GetComponent(const RID& entity) const {
		return m_ComponentManager.GetComponent<T>(entity);
	}

	std::vector<Component*> GetComponents(const RID& entity) const {
		return m_ComponentManager.GetComponents(entity);
	}

	template <ComponentType T>
	std::vector<T*> GetAllComponentsOfType() const {
		return m_ComponentManager.GetAllComponentsOfType<T>();
	}
#pragma endregion

private:
	ComponentManager m_ComponentManager;
	Camera* m_MainCamera;
	std::string m_Name;
	std::unordered_map<RID, Entity*> m_Entities;
};
}
