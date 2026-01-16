#pragma once

#include <typeindex>
#include <string_view>

#include "origo/components/NativeComponentManager.h"
#include "origo/scripting/ScriptComponentManager.h"
#include "origo/scene/SceneCommand.h"

namespace Origo {
class Scene;

namespace SceneSerializer {
	void SerializeToFile(Scene& scene, const std::filesystem::path& out);
	Scope<Scene> DeserializeFromFile(const std::filesystem::path& path);
}

class Scene {
	friend void SceneSerializer::SerializeToFile(Scene& scene, const std::filesystem::path& out);
	friend Scope<Scene> SceneSerializer::DeserializeFromFile(const std::filesystem::path& path);

public:
	Scene(std::string_view name = "SampleScene");
	Scene(const Scene& other);
	~Scene();
	Scene& operator=(const Scene&) = delete;

	RID CreateEntity(std::string_view name);
	void ScheduleRemoveEntity(const RID&);
	void ScheduleRemoveNativeComponent(const RID&, const std::type_index&);

	const std::vector<RID>& GetEntities() const { return m_Entities; }
	const std::string& GetName() const { return m_Name; }

	void EndFrame();

#pragma region FORWARDING
	template <ComponentType T, typename... Args>
	T* AddNativeComponent(const RID& entity, Args&&... args) {
		return &m_NativeComponentManager.AddComponent<T>(
		    entity,
		    std::forward<Args>(args)...);
	}

	bool AddNativeComponent(const RID& e, std::type_index type) {
		return m_NativeComponentManager.AddComponentByType(e, type);
	}

	void AddScriptComponent(RID entity, ScriptComponentID type) {
		m_ScriptComponentManager.Add(entity, type);
	}

	template <ComponentType T>
	T* GetNativeComponent(const RID& entity) {
		return m_NativeComponentManager.GetComponent<T>(entity);
	}

	ScriptComponentInstance* GetScriptComponent(RID entity, ScriptComponentID type) {
		return m_ScriptComponentManager.Get(entity, type);
	}

	const ScriptComponentInstance* GetScriptComponent(RID entity, ScriptComponentID type) const {
		return m_ScriptComponentManager.Get(entity, type);
	}

	template <ComponentType T>
	const T* GetNativeComponent(const RID& entity) const {
		return m_NativeComponentManager.GetComponent<T>(entity);
	}

	template <ComponentType T>
	bool HasNativeComponent(const RID& entity) const {
		return m_NativeComponentManager.HasComponent<T>(entity);
	}

	bool HasScriptComponent(RID entity, ScriptComponentID type) const {
		return m_ScriptComponentManager.Has(entity, type);
	}

	template <ComponentType T, typename Func>
	void ForEach(Func&& func) {
		m_NativeComponentManager.ForEach<T>(std::forward<Func>(func));
	}

	template <ComponentType T, typename Func>
	void ForEach(Func&& func) const {
		m_NativeComponentManager.ForEach<T>(std::forward<Func>(func));
	}

	template <ComponentType... Ts, typename Func>
	void View(Func&& func) {
		m_NativeComponentManager.View<Ts...>(std::forward<Func>(func));
	}

	template <ComponentType... Ts, typename Func>
	void View(Func&& func) const {
		m_NativeComponentManager.View<Ts...>(std::forward<Func>(func));
	}

	void* GetNativeComponentByType(const RID& entity, std::type_index type) {
		return m_NativeComponentManager.GetComponentByType(entity, type);
	}

	const void* GetNativeComponentByType(const RID& entity, std::type_index type) const {
		return m_NativeComponentManager.GetComponentByType(entity, type);
	}

	bool HasNativeComponentByType(const RID& entity, std::type_index type) const {
		return m_NativeComponentManager.HasComponent(entity, type);
	}

#pragma endregion

private:
	void Flush();
	void RemoveEntity(const RID&);

private:
	std::string m_Name;

	NativeComponentManager m_NativeComponentManager;
	ScriptComponentManager m_ScriptComponentManager;
	std::vector<SceneCommand*> m_Commands;
	std::vector<RID> m_Entities;
};

}
