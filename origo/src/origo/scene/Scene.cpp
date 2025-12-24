#include "origo/scene/Scene.h"
#include "origo/scene/CameraComponent.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/Transform.h"

namespace Origo {
Scene::Scene(std::string_view name)
    : m_Name(name) {
	auto cam { CreateEntity("Main Camera") };
	AddComponent<Transform>(cam);
	AddComponent<CameraComponent>(cam);
}

Scene::Scene(const Scene& other)
    : m_Name(other.GetName()) {
	for (const auto& [rid, entity] : other.m_Entities) {
		Entity* cloned = new Entity(*entity);
		m_Entities.emplace(rid, cloned);
	}

	m_ComponentManager.CloneFrom(other.m_ComponentManager);
}

Entity* Scene::CreateEntity(std::string_view name) {
	auto entity = new Entity(name);
	m_Entities.emplace(entity->GetId(), entity);
	return entity;
}

Scene::~Scene() {
	for (auto& [_, e] : m_Entities) {
		delete e;
	}
};

}
