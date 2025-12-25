#pragma once

#include <typeindex>
#include <string_view>

#include "origo/scene/NativeComponentManager.h"
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
	~Scene();
	Scene& operator=(const Scene&) = delete;

	RID CreateEntity(std::string_view name);

	const std::vector<RID>& GetEntities() const { return m_Entities; }
	const std::string& GetName() const { return m_Name; }

#pragma region FORWARDING
	template <ComponentType T, typename... Args>
	T* AddComponent(const RID& entity, Args&&... args) {
		return &m_ComponentManager.AddComponent<T>(
		    entity,
		    std::forward<Args>(args)...);
	}

	bool AddComponent(const RID& e, std::type_index type) {
		return m_ComponentManager.AddComponentByType(e, type);
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

	void* GetComponentByType(const RID& entity, std::type_index type) {
		return m_ComponentManager.GetComponentByType(entity, type);
	}

	const void* GetComponentByType(const RID& entity, std::type_index type) const {
		return m_ComponentManager.GetComponentByType(entity, type);
	}

	bool HasComponentByType(const RID& entity, std::type_index type) const {
		return m_ComponentManager.HasComponent(entity, type);
	}

#pragma endregion

private:
	std::string m_Name;

	NativeComponentManager m_ComponentManager;
	std::vector<RID> m_Entities;
};

}
