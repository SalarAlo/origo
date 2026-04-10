#include <algorithm>

#include "origo/scene/Scene.h"

#include "origo/components/Name.h"
#include "origo/components/Transform.h"

#include "origo/core/Typedefs.h"

#include "origo/physics/PhysicsWorld.h"

#include "origo/scene/SceneCommand.h"

namespace Origo {

Scene::Scene(std::string_view name)
    : m_name(name)
    , m_physics_world(make_scope<PhysicsWorld>()) {
}

Scene::Scene(const Scene& other) {
	m_entities = other.m_entities;

	m_native_component_manager = other.m_native_component_manager;
	m_script_component_manager = other.m_script_component_manager;
	m_physics_world = make_scope<PhysicsWorld>();
}

RID Scene::create_entity(std::string_view name) {
	return create_entity_with_rid(RID::new_rid(), name);
}

std::optional<RID> Scene::duplicate_entity(const RID& source, std::string_view name) {
	const auto source_it = std::find(m_entities.begin(), m_entities.end(), source);

	if (source_it == m_entities.end())
		return std::nullopt;

	std::string duplicate_name;
	if (!name.empty()) {
		duplicate_name = name;
	} else if (const auto* source_name = get_native_component<NameComponent>(source)) {
		duplicate_name = source_name->Name + " Copy";
	} else {
		duplicate_name = "Entity Copy";
	}

	RID duplicate = create_entity(duplicate_name);
	m_native_component_manager.copy_components(source, duplicate);
	m_script_component_manager.copy_components(source, duplicate);

	if (auto* name_component = get_native_component<NameComponent>(duplicate))
		name_component->Name = duplicate_name;

	return duplicate;
}

RID Scene::create_entity_with_rid(const RID& rid, std::string_view name) {
	m_entities.push_back(rid);
	add_native_component<NameComponent>(m_entities.back(), name);
	add_native_component<TransformComponent>(m_entities.back());
	return m_entities.back();
}

bool Scene::has_entity(const RID& entity) const {
	return std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end();
}

void Scene::schedule_remove_entity(const RID& rid) {
	m_commands.emplace_back(make_scope<RemoveEntityCommand>(rid));
}

void Scene::schedule_remove_native_component(const RID& entity, const std::type_index& type) {
	m_commands.emplace_back(make_scope<RemoveEntityNativeComponentCommand>(entity, type));
}

void Scene::flush_commands() {
	for (auto& cmd : m_commands) {
		switch (cmd->Type) {
		case SceneCommandType::RemoveEntity: {
			auto remove_cmd { static_cast<RemoveEntityCommand*>(cmd.get()) };
			remove_entity(remove_cmd->EntityToRemove);
			break;
		}
		case SceneCommandType::RemoveEntityNativeComponent: {
			auto remove_cmd { static_cast<RemoveEntityNativeComponentCommand*>(cmd.get()) };
			m_native_component_manager.remove_component_by_type(remove_cmd->EntityToRemoveComponent, remove_cmd->ComponentToRemove);
			break;
		}
		}
	}

	m_commands.clear();
}

void Scene::end_frame() {
	flush_commands();
}

PhysicsWorld& Scene::get_physics_world() {
	return *m_physics_world;
}

const PhysicsWorld& Scene::get_physics_world() const {
	return *m_physics_world;
}

void Scene::remove_entity(const RID& rid) {
	m_native_component_manager.remove_all_components(rid);
	m_script_component_manager.remove_all_components(rid);

	auto it = std::remove(m_entities.begin(), m_entities.end(), rid);
	m_entities.erase(it, m_entities.end());
}

Scene::~Scene() {
	while (!m_entities.empty())
		remove_entity(m_entities.back());
};

}
