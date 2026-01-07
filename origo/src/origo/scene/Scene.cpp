#include "origo/scene/Scene.h"
#include "origo/scene/Transform.h"
#include "origo/scene/Name.h"

namespace Origo {
Scene::Scene(std::string_view name)
    : m_Name(name) {
}

Scene::Scene(const Scene& other)
    : m_Name(other.GetName()) {
	for (const auto& entity : other.m_Entities) {
		m_Entities.push_back(entity);
	}

	m_NativeComponentManager.CloneFrom(other.m_NativeComponentManager);
	m_ScriptComponentManager = other.m_ScriptComponentManager;
}

RID Scene::CreateEntity(std::string_view name) {
	m_Entities.push_back(RID::New());
	AddNativeComponent<Name>(m_Entities.back())->SetName(name);
	AddNativeComponent<Transform>(m_Entities.back());
	return m_Entities.back();
}

Scene::~Scene() {
};

}
