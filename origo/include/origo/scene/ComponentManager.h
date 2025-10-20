#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"
#include <concepts>
#include <unordered_map>

namespace Origo {

template <typename T>
concept ComponentConcept = std::derived_from<T, Component>;

class ComponentManager {
public:
	template <ComponentConcept T, typename... Args>
	Ref<T> AddComponent(const Ref<Entity>& entity, Args&&... args) {
		auto& map = GetComponentsMap<T>();
		auto component = MakeRef<T>(std::forward<Args>(args)..., entity);
		map[entity->GetId()] = component;

		return component;
	}

	template <ComponentConcept T>
	Ref<T> GetComponent(const Ref<Entity>& entity) {
		auto& map = GetComponentsMap<T>();
		return map[entity->GetId()];
	}

	template <ComponentConcept T>
	std::vector<Ref<T>> GetAllComponentsOfType() {
		auto& map = GetComponentsMap<T>();
		std::vector<Ref<T>> result;
		result.reserve(map.size());
		for (auto& [_, component] : map)
			result.push_back(component);
		return result;
	}

private:
	template <ComponentConcept T>
	std::unordered_map<UUID, Ref<T>>& GetComponentsMap() {
		static std::unordered_map<UUID, Ref<T>> m_Storage {};
		return m_Storage;
	}

private:
};

}
