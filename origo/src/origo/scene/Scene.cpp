#include "origo/scene/Scene.h"
#include "origo/renderer/Renderer.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/MeshRenderer.h"
#include "origo/scene/ModelRenderer.h"

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

void Scene::Render() {
	for (const auto& renderer : m_ComponentManager.GetAllComponentsOfType<MeshRenderer>()) {
		Renderer::Submit(
		    renderer->GetMesh(),
		    renderer->GetMaterial(),
		    m_ComponentManager.GetComponent<Transform>(renderer->AttachedTo));
	}

	for (const auto& renderer : m_ComponentManager.GetAllComponentsOfType<ModelRenderer>()) {
		for (const auto& submesh : renderer->GetModel()->GetSubMeshes()) {
			Renderer::Submit(
			    submesh.mesh,
			    submesh.material,
			    m_ComponentManager.GetComponent<Transform>(renderer->AttachedTo));
		}
	}

	Renderer::Flush(m_Camera);
}

}
