#include "origo/scene/Scene.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/MeshRenderer.h"

namespace Origo {

Ref<Entity> Scene::CreateEntity() {
	auto entity = MakeRef<Entity>();
	m_Entities.emplace_back(entity);
	return entity;
}

void Scene::Render() {
	for (const auto& e : m_Entities) {
		if (Ref<MeshRenderer> x = e->GetComponent<MeshRenderer>()) {
			m_Renderer.Submit(
			    x->GetMesh(), x->GetMaterial(), x->E.GetComponent<Transform>());
		}
	}

	m_Renderer.Flush(m_Camera);
}

Camera& Scene::GetCamera() {
	return m_Camera;
}

}
