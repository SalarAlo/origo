#include "origo/scene/Scene.h"
#include "origo/Camera.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
Scene::Scene(std::string_view name, float ar)
    : m_Name(name) {
	auto camera { CreateEntity("MainCamera") };
	m_MainCamera = m_ComponentManager.AddComponent<Camera>(camera, ar);
}

Ref<Entity> Scene::CreateEntity(std::string_view name) {
	auto entity = MakeRef<Entity>(name);
	m_Entities.emplace(entity->GetId(), entity);
	return entity;
}

}
