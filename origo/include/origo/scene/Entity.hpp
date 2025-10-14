#pragma once

#include <concepts>
#include <typeindex>

#include "Component.h"

namespace Origo {
template <typename T>
concept ComponentConcept = std::derived_from<T, Origo::Component>;

class Entity {
public:
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

private:
	std::unordered_map<std::type_index, Ref<Component>> m_Components;
};

}
