#include "origo/scene/Scene.h"
#include "origo/Camera.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
Scene::Scene(std::string_view name)
    : m_Name(name) {
	auto camera { CreateEntity("MainCamera") };
	m_MainCamera = m_ComponentManager.AddComponent<Camera>(camera);
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
