#pragma once

#include <optional>
#include <string_view>
#include <typeindex>

#include "origo/components/NativeComponentManager.h"

#include "origo/scene/SceneCommand.h"

#include "origo/scripting/ComponentDataManager.h"
#include "origo/scripting/ScriptAttachmentManager.h"
#include "origo/scripting/ScriptExecutionSystem.h"
#include "origo/scripting/ScriptRegistry.h"

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
	bool has_entity(const RID& entity) const;
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

	void add_component_data(RID entity, ComponentTypeID type) {
		m_component_data_manager.add(entity, type);
		for (const auto& [script_id, script] : ScriptRegistry::get_all()) {
			if (script.ComponentType.Type == type)
				add_script(entity, script_id);
		}
	}

	void add_script(RID entity, ScriptTypeID type) {
		m_script_attachment_manager.add(entity, type);
		if (auto* attachment = m_script_attachment_manager.get(entity, type))
			m_script_execution_system.on_script_added(entity, *attachment);
	}

	void sync_component_scripts() {
		for (const auto& [entity, components] : m_component_data_manager.get_all()) {
			for (const auto& component : components) {
				for (const auto& [script_id, script] : ScriptRegistry::get_all()) {
					if (script.ComponentType.Type == component.ID && !has_script(entity, script_id))
						add_script(entity, script_id);
				}
			}
		}
	}

	template <ComponentType T>
	T* get_native_component(const RID& entity) {
		return m_native_component_manager.get_component<T>(entity);
	}

	ComponentInstanceData* get_component_data(RID entity, ComponentTypeID type) {
		return m_component_data_manager.get(entity, type);
	}

	ScriptAttachmentData* get_script(RID entity, ScriptTypeID type) {
		return m_script_attachment_manager.get(entity, type);
	}

	const ComponentInstanceData* get_component_data(RID entity, ComponentTypeID type) const {
		return m_component_data_manager.get(entity, type);
	}

	const ScriptAttachmentData* get_script(RID entity, ScriptTypeID type) const {
		return m_script_attachment_manager.get(entity, type);
	}

	const ReflectedFieldValue* find_component_data_field(RID entity, std::string_view name) const {
		return m_component_data_manager.find_field(entity, name);
	}

	const ReflectedFieldValue* find_component_data_field(RID entity, ComponentTypeID type, std::string_view name) const {
		const auto* component = m_component_data_manager.get(entity, type);
		if (!component)
			return nullptr;

		for (const auto& field : component->Fields) {
			if (field.Name == name)
				return &field;
		}
		return nullptr;
	}

	ReflectedFieldValue* find_component_data_field(RID entity, ComponentTypeID type, std::string_view name) {
		auto* component = m_component_data_manager.get(entity, type);
		if (!component)
			return nullptr;

		for (auto& field : component->Fields) {
			if (field.Name == name)
				return &field;
		}
		return nullptr;
	}

	template <ComponentType T>
	const T* get_native_component(const RID& entity) const {
		return m_native_component_manager.get_component<T>(entity);
	}

	template <ComponentType T>
	bool has_native_component(const RID& entity) const {
		return m_native_component_manager.has_component<T>(entity);
	}

	bool has_component_data(RID entity, ComponentTypeID type) const {
		return m_component_data_manager.has(entity, type);
	}

	bool has_script(RID entity, ScriptTypeID type) const {
		return m_script_attachment_manager.has(entity, type);
	}

	bool remove_component_data_if_exists(RID entity, ComponentTypeID type) {
		for (const auto& [script_id, script] : ScriptRegistry::get_all()) {
			if (script.ComponentType.Type == type)
				remove_script_if_exists(entity, script_id);
		}
		return m_component_data_manager.remove_if_exists(entity, type);
	}

	bool remove_script_if_exists(RID entity, ScriptTypeID type) {
		m_script_execution_system.on_script_removed(entity, type);
		return m_script_attachment_manager.remove_if_exists(entity, type);
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

	void start_scripts() {
		sync_component_scripts();
		m_script_execution_system.start_scene(*this, m_script_attachment_manager);
	}

	void stop_scripts() {
		m_script_execution_system.stop_scene();
	}

	void update_scripts(float dt) {
		m_script_execution_system.update(dt);
	}

	void fixed_update_scripts(float dt) {
		m_script_execution_system.fixed_update(dt);
	}

#pragma endregion

private:
	RID create_entity_with_rid(const RID& rid, std::string_view name = "Entity");
	void flush_commands();
	void remove_entity(const RID&);

private:
	std::string m_name;

	NativeComponentManager m_native_component_manager {};
	ComponentDataManager m_component_data_manager {};
	ScriptAttachmentManager m_script_attachment_manager {};
	ScriptExecutionSystem m_script_execution_system {};
	Scope<PhysicsWorld> m_physics_world {};

	std::vector<Scope<SceneCommand>> m_commands {};
	std::vector<RID> m_entities {};
};

}
