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

}
