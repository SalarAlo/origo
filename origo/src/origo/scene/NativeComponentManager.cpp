#include "origo/scene/NativeComponentManager.h"
#include "origo/scene/NativeComponentRegistry.h"

namespace Origo {

NativeComponentManager::NativeComponentManager(const NativeComponentManager& other) {
	CloneFrom(other);
}

NativeComponentManager& NativeComponentManager::operator=(const NativeComponentManager& other) {
	if (this != &other)
		CloneFrom(other);
	return *this;
}

void NativeComponentManager::CloneFrom(const NativeComponentManager& other) {
	m_Storages.clear();
	m_Storages.reserve(other.m_Storages.size());

	for (const auto& [type, storage] : other.m_Storages) {
		m_Storages.emplace(type, storage->Clone());
	}
}

bool NativeComponentManager::CanAddComponentByType(std::type_index type) {
	return NativeComponentRegistry::Get(type) != nullptr;
}

bool NativeComponentManager::AddComponentByType(const RID& entity, std::type_index type) {
	auto* info = NativeComponentRegistry::Get(type);
	if (!info)
		return false;

	if (info->Has(*this, entity))
		return false;

	info->Add(*this, entity);
	return true;
}

std::size_t NativeComponentManager::RemoveAllComponents(const RID& entity) {
	std::size_t removed = 0;
	for (auto& [_, storage] : m_Storages) {
		removed += storage->Remove(entity) ? 1 : 0;
	}
	return removed;
}

bool NativeComponentManager::HasComponent(const RID& entity, std::type_index type) const {
	auto it = m_Storages.find(type);
	return it != m_Storages.end() && it->second->Has(entity);
}

bool NativeComponentManager::RemoveComponentByType(
    const RID& entity,
    std::type_index type) {

	auto it = m_Storages.find(type);
	if (it == m_Storages.end())
		return false;

	return it->second->Remove(entity);
}

}
