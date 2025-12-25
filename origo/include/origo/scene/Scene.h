#pragma once

#include <typeindex>
#include <string_view>
#include <unordered_map>

#include "origo/scene/Entity.hpp"
#include "origo/scene/ComponentManager.h"
#include "origo/serialization/ISerializer.h"

namespace Origo {
class Scene;

namespace SceneSerialization {
	void Serialize(const Scene& scene, ISerializer& backend);
}

class Scene {
	friend void SceneSerialization::Serialize(const Scene& scene, ISerializer& backend);

public:
	Scene(std::string_view name = "SampleScene");
	Scene(const Scene& other);
	Scene& operator=(const Scene&) = delete;
	~Scene();

	Entity* CreateEntity(std::string_view name);

	const std::unordered_map<RID, Entity*>& GetEntities() const { return m_Entities; }
	const std::string& GetName() const { return m_Name; }

#pragma region FORWARDING
	template <ComponentType T, typename... Args>
	T* AddComponent(Entity* e, Args&&... args) {
		return &m_ComponentManager.AddComponent<T>(
		    e->GetId(),
		    std::forward<Args>(args)...);
	}

	template <ComponentType T>
	T* GetComponent(const RID& entity) {
		return m_ComponentManager.GetComponent<T>(entity);
	}

	template <ComponentType T>
	const T* GetComponent(const RID& entity) const {
		return m_ComponentManager.GetComponent<T>(entity);
	}

	template <ComponentType T>
	bool HasComponent(const RID& entity) const {
		return m_ComponentManager.HasComponent<T>(entity);
	}

	template <ComponentType T, typename Func>
	void ForEach(Func&& func) {
		m_ComponentManager.ForEach<T>(std::forward<Func>(func));
	}

	template <ComponentType T, typename Func>
	void ForEach(Func&& func) const {
		m_ComponentManager.ForEach<T>(std::forward<Func>(func));
	}

	template <ComponentType... Ts, typename Func>
	void View(Func&& func) {
		m_ComponentManager.View<Ts...>(std::forward<Func>(func));
	}

	template <ComponentType... Ts, typename Func>
	void View(Func&& func) const {
		m_ComponentManager.View<Ts...>(std::forward<Func>(func));
	}

	void* GetComponentByType(RID entity, std::type_index type) {
		return m_ComponentManager.GetComponentByType(entity, type);
	}

	const void* GetComponentByType(RID entity, std::type_index type) const {
		return m_ComponentManager.GetComponentByType(entity, type);
	}

	bool HasComponentByType(RID entity, std::type_index type) const {
		return m_ComponentManager.HasComponent(entity, type);
	}

#pragma endregion

private:
	std::string m_Name;

	ComponentManager m_ComponentManager;
	std::unordered_map<RID, Entity*> m_Entities;
};

}
