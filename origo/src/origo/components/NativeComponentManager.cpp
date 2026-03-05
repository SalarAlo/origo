#include "NativeComponentManager.h"

#include "NativeComponentRegistry.h"

namespace Origo {

NativeComponentManager::NativeComponentManager(const NativeComponentManager& other) {
	clone_from(other);
}

NativeComponentManager& NativeComponentManager::operator=(const NativeComponentManager& other) {
	if (this != &other)
		clone_from(other);
	return *this;
}

void NativeComponentManager::clone_from(const NativeComponentManager& other) {
	m_storages.clear();
	m_storages.reserve(other.m_storages.size());

	for (const auto& [type, storage] : other.m_storages) {
		m_storages.emplace(type, storage->clone());
	}
}

bool NativeComponentManager::can_add_component_by_type(std::type_index type) {
	return NativeComponentRegistry::get_instance().get_component_info(type) != nullptr;
}

bool NativeComponentManager::add_component_by_type(const RID& entity, std::type_index type) {
	auto* info = NativeComponentRegistry::get_instance().get_component_info(type);
	if (!info)
		return false;

	if (info->Has(*this, entity))
		return false;

	info->Add(*this, entity);
	return true;
}

std::size_t NativeComponentManager::remove_all_components(const RID& entity) {
	std::size_t removed = 0;
	for (auto& [_, storage] : m_storages) {
		removed += storage->remove(entity) ? 1 : 0;
	}
	return removed;
}

bool NativeComponentManager::has_component(const RID& entity, std::type_index type) const {
	auto it = m_storages.find(type);
	return it != m_storages.end() && it->second->has(entity);
}

bool NativeComponentManager::remove_component_by_type(
    const RID& entity,
    std::type_index type) {

	auto it = m_storages.find(type);
	if (it == m_storages.end())
		return false;

	return it->second->remove(entity);
}

void NativeComponentManager::for_each_component_on_entity(const RID& entity, VisitFn fn, void* user) {
	for (const auto& [type, info] : NativeComponentRegistry::get_instance().get_all()) {
		if (!info.Has(*this, entity))
			continue;

		void* ptr = info.Get(*this, entity);
		if (!ptr)
			continue;

		fn(info, ptr, user);
	}
}

void NativeComponentManager::for_each_component_on_entity(const RID& entity, VisitFnConst fn, void* user) const {
	for (const auto& [type, info] : NativeComponentRegistry::get_instance().get_all()) {
		if (!info.Has(*this, entity))
			continue;

		const void* ptr = this->get_component_by_type(entity, type);
		if (!ptr)
			continue;

		fn(info, ptr, user);
	}
}

void NativeComponentManager::serialize_entity(const RID& entity, ISerializer& backend) const {
	backend.begin_array("native_components");

	for_each_component_on_entity(entity, [&](const NativeComponentTypeInfo& info, const void* c) {
		if (!info.Serializer)
			return;
		backend.begin_array_object();

		backend.write("type", info.DisplayName);

		backend.begin_object("data");
		info.Serializer->serialize(static_cast<Component*>(const_cast<void*>(c)), backend);
		backend.end_object();

		backend.end_array_object();
	});

	backend.end_array();
}

}
