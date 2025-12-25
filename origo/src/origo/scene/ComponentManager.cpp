#include "origo/scene/ComponentManager.h"
#include "origo/scene/ComponentRegistry.h"

namespace Origo {

ComponentManager::ComponentManager(const ComponentManager& other) {
	CloneFrom(other);
}

ComponentManager& ComponentManager::operator=(const ComponentManager& other) {
	if (this != &other)
		CloneFrom(other);
	return *this;
}

void ComponentManager::CloneFrom(const ComponentManager& other) {
	m_Storages.clear();
	m_Storages.reserve(other.m_Storages.size());

	for (const auto& [type, storage] : other.m_Storages) {
		m_Storages.emplace(type, storage->Clone());
	}
}

bool ComponentManager::CanAddComponentByType(std::type_index type) {
	return ComponentRegistry::Get(type) != nullptr;
}

bool ComponentManager::AddComponentByType(RID entity, std::type_index type) {
	auto* info = ComponentRegistry::Get(type);
	if (!info)
		return false;

	if (info->Has(*this, entity))
		return false;

	info->Add(*this, entity);
	return true;
}

}
