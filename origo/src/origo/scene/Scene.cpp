#include "origo/scene/Scene.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/MeshRenderer.h"

namespace Origo {
Scene::Scene(std::string_view name)
    : m_Name(name) { }

Ref<Entity> Scene::CreateEntity(std::string_view name) {
	auto entity = MakeRef<Entity>(name);
	m_Entities.emplace(entity->GetId(), entity);
	return entity;
}

void Scene::Render() {
	for (const auto& renderer : m_ComponentManager.GetAllComponentsOfType<MeshRenderer>()) {
		Renderer::Submit(
		    renderer->GetMesh(),
		    renderer->GetMaterial(),
		    m_ComponentManager.GetComponent<Transform>(renderer->AttachedTo));
	}

	Renderer::Flush(m_Camera);
}

const std::string& Scene::GetName() const { return m_Name; }

Camera& Scene::GetCamera() {
	return m_Camera;
}

}
