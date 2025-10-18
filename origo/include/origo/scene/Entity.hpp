#pragma once

#include <concepts>
#include <typeindex>

#include "Component.h"

namespace Origo {
template <typename T>
concept ComponentConcept = std::derived_from<T, Origo::Component>;

struct EntityId {
	uint32_t Index {};
	uint32_t Generation {};

	bool operator==(const EntityId& other) const noexcept {
		return Index == other.Index && Generation == other.Generation;
	}
};

struct EntityIdHash {
	size_t operator()(const EntityId& id) const noexcept {
		return (size_t(id.Index) << 32) ^ size_t(id.Generation);
	}
};

class Entity {

public:
	Entity(std::string_view name, EntityId entityId)
	    : m_Name(name)
	    , m_Id(entityId) {
	}

	template <ComponentConcept T, typename... Args>
	Ref<T> AttachComponent(Args&&... args) {
		Ref<T> newComponent { MakeRef<T>(std::forward<Args>(args)..., *this) };
		m_Components[typeid(T)] = newComponent;
		return newComponent;
	}

	template <typename T>
	Ref<T> GetComponent() {
		auto it = m_Components.find(typeid(T));
		if (it != m_Components.end())
			return std::dynamic_pointer_cast<T>(it->second);

		return nullptr;
	}

	const EntityId& GetEntityId() const {
		return m_Id;
	}
	const std::string& GetName() const {
		return m_Name;
	}

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	Entity(Entity&&) noexcept = default;
	Entity& operator=(Entity&&) noexcept = default;

private:
	std::string m_Name {};
	EntityId m_Id {};
	std::unordered_map<std::type_index, Ref<Component>> m_Components {};
};

}
