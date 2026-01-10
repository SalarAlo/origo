#include "origo/scene/Scene.h"
#include "origo/scene/SceneCommand.h"
#include "origo/scene/Transform.h"
#include "origo/scene/Name.h"
#include <algorithm>

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

void Scene::ScheduleRemoveEntity(const RID& rid) {
	m_Commands.emplace_back(new RemoveEntityCommand { rid });
}

void Scene::ScheduleRemoveNativeComponent(const RID& entity, const std::type_index& type) {

	m_Commands.emplace_back(new RemoveEntityNativeComponentCommand { entity, type });
}

void Scene::Flush() {
	for (auto& cmd : m_Commands) {
		switch (cmd->Type) {
		case SceneCommandType::RemoveEntity: {
			auto removeCmd { static_cast<RemoveEntityCommand*>(cmd) };
			RemoveEntity(removeCmd->EntityToRemove);
		}
		case SceneCommandType::RemoveEntityNativeComponent: {
			auto removeCmd { static_cast<RemoveEntityNativeComponentCommand*>(cmd) };
			m_NativeComponentManager.RemoveComponentByType(removeCmd->EntityToRemoveComponent, removeCmd->ComponentToRemove);
		}
		}

		delete cmd;
	}

	m_Commands.clear();
}

void Scene::EndFrame() {
	Flush();
}

void Scene::RemoveEntity(const RID& rid) {
	m_NativeComponentManager.RemoveAllComponents(rid);

	auto it = std::remove(m_Entities.begin(), m_Entities.end(), rid);
	m_Entities.erase(it, m_Entities.end());
}

Scene::~Scene() { };

}
