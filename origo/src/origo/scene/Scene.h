#pragma once

#include <optional>
#include <string_view>
#include <typeindex>

#include "origo/components/NativeComponentManager.h"

#include "origo/scene/SceneCommand.h"

#include "origo/scripting/ScriptComponentManager.h"

namespace Origo {
class Scene;
class PhysicsWorld;
class SceneEntitySerializationManager;

namespace SceneSerializer {
	void serialize_to_file(Scene& scene, const std::filesystem::path& out);
	Scope<Scene> deserialize_from_file(const std::filesystem::path& path);
}

class Scene {
	friend void SceneSerializer::serialize_to_file(Scene& scene, const std::filesystem::path& out);
	friend Scope<Scene> SceneSerializer::deserialize_from_file(const std::filesystem::path& path);
	friend class SceneEntitySerializationManager;

public:
	Scene(std::string_view name = "SampleScene");
	Scene(const Scene& other);
	~Scene();
	Scene& operator=(const Scene&) = delete;

	void end_frame();
	const std::string& get_name() const { return m_name; }
	PhysicsWorld& get_physics_world();
	const PhysicsWorld& get_physics_world() const;

	RID create_entity(std::string_view name);
	std::optional<RID> duplicate_entity(const RID& source, std::string_view name = {});
	const std::vector<RID>& get_entities() const { return m_entities; }
	void schedule_remove_entity(const RID&);

	void schedule_remove_native_component(const RID&, const std::type_index&);

#pragma region FORWARDING
	template <ComponentType T, typename... Args>
	T* add_native_component(const RID& entity, Args&&... args) {
		return &m_native_component_manager.add_component<T>(
		    entity,
		    std::forward<Args>(args)...);
	}

	bool add_native_component(const RID& e, std::type_index type) {
		return m_native_component_manager.add_component_by_type(e, type);
	}

	void add_script_component(RID entity, ScriptComponentID type) {
		m_script_component_manager.add(entity, type);
	}

	template <ComponentType T>
	T* get_native_component(const RID& entity) {
		return m_native_component_manager.get_component<T>(entity);
	}

	ScriptComponentInstance* get_script_component(RID entity, ScriptComponentID type) {
		return m_script_component_manager.get(entity, type);
	}

	const ScriptComponentInstance* get_script_component(RID entity, ScriptComponentID type) const {
		return m_script_component_manager.get(entity, type);
	}

	template <ComponentType T>
	const T* get_native_component(const RID& entity) const {
		return m_native_component_manager.get_component<T>(entity);
	}

	template <ComponentType T>
	bool has_native_component(const RID& entity) const {
		return m_native_component_manager.has_component<T>(entity);
	}

	bool has_script_component(RID entity, ScriptComponentID type) const {
		return m_script_component_manager.has(entity, type);
	}

	template <ComponentType T, typename Func>
	void for_each(Func&& func) {
		m_native_component_manager.for_each<T>(std::forward<Func>(func));
	}

	template <ComponentType T, typename Func>
	void for_each(Func&& func) const {
		m_native_component_manager.for_each<T>(std::forward<Func>(func));
	}

	template <ComponentType... Ts, typename Func>
	void view(Func&& func) {
		m_native_component_manager.view<Ts...>(std::forward<Func>(func));
	}

	template <ComponentType... Ts, typename Func>
	void view(Func&& func) const {
		m_native_component_manager.view<Ts...>(std::forward<Func>(func));
	}

	void* get_native_component_by_type(const RID& entity, std::type_index type) {
		return m_native_component_manager.get_component_by_type(entity, type);
	}

	const void* get_native_component_by_type(const RID& entity, std::type_index type) const {
		return m_native_component_manager.get_component_by_type(entity, type);
	}

	bool has_native_component_by_type(const RID& entity, std::type_index type) const {
		return m_native_component_manager.has_component(entity, type);
	}

#pragma endregion

private:
	RID create_entity_with_rid(const RID& rid, std::string_view name = "Entity");
	void flush_commands();
	void remove_entity(const RID&);

private:
	std::string m_name;

	NativeComponentManager m_native_component_manager {};
	ScriptComponentManager m_script_component_manager {};
	Scope<PhysicsWorld> m_physics_world {};

	std::vector<Scope<SceneCommand>> m_commands {};
	std::vector<RID> m_entities {};
};

}
