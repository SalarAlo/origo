#include "origo/scene/Scene.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/MeshRenderer.h"

namespace Origo {
Scene::Scene(std::string_view name)
    : m_Name(name) { }

Ref<Entity> Scene::CreateEntity(std::string_view name) {
	EntityId entityId;

	if (m_FreeIndices.empty()) {
		m_Generation.push_back(0);

		entityId.Index = m_Generation.size() - 1;
		entityId.Generation = 0;
	} else {
		auto freeIdx { m_FreeIndices.front() };
		m_FreeIndices.pop();
		entityId.Index = freeIdx;
		entityId.Generation = m_Generation[freeIdx];
	}

	auto entity = MakeRef<Entity>(name, entityId);
	m_Entities.emplace(entityId, entity);
	return entity;
}

void Scene::Render() {
	for (const auto& [_, e] : m_Entities) {
		if (Ref<MeshRenderer> x = e->GetComponent<MeshRenderer>()) {
			Renderer::Submit(
			    x->GetMesh(), x->GetMaterial(), x->E.GetComponent<Transform>());
		}
	}

	Renderer::Flush(m_Camera);
}

const std::string& Scene::GetName() const { return m_Name; }

Camera& Scene::GetCamera() {
	return m_Camera;
}

}
