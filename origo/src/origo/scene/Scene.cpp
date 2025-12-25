#include "origo/scene/Scene.h"
#include "origo/scene/CameraComponent.h"
#include "origo/scene/Transform.h"
#include "origo/scene/Name.h"

namespace Origo {
Scene::Scene(std::string_view name)
    : m_Name(name) {
	auto cam { CreateEntity("Main Camera") };
	auto transfComp { AddComponent<Transform>(cam) };
	auto cameraComp { AddComponent<CameraComponent>(cam) };
	cameraComp->IsPrimary = true;
	transfComp->SetPosition({ -3.5, 4, 5 });
	transfComp->SetRotation({ -33, -33, 0 });
}

Scene::Scene(const Scene& other)
    : m_Name(other.GetName()) {
	for (const auto& entity : other.m_Entities) {
		m_Entities.push_back(entity);
	}

	m_ComponentManager.CloneFrom(other.m_ComponentManager);
}

RID Scene::CreateEntity(std::string_view name) {
	m_Entities.push_back(RID::New());
	AddComponent<Name>(m_Entities.back())->SetName(name);
	return m_Entities.back();
}

Scene::~Scene() {
};

}
