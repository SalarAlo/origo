#include "origo/scene/Scene.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
Scene::Scene(std::string_view name, float ar)
    : m_Name(name) {
	m_Camera.SetAspectResolutino(ar);
}

Ref<Entity> Scene::CreateEntity(std::string_view name) {
	auto entity = MakeRef<Entity>(name);
	m_Entities.emplace(entity->GetId(), entity);
	return entity;
}

}
