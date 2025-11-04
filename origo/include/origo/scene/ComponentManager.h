#include "origo/core/Identifiable.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"
#include <concepts>
#include <typeindex>
#include <unordered_map>

namespace Origo {

template <typename T>
concept ComponentConcept = std::derived_from<T, Component>;

class ComponentManager {
public:
	template <ComponentConcept T, typename... Args>
	Ref<T> AddComponent(const Ref<Entity>& entity, Args&&... args) {
		auto& map = GetComponentsMap<T>();
		auto component = MakeRef<T>(entity, std::forward<Args>(args)...);
		map[entity->GetId()] = component;

		return component;
	}

	template <ComponentConcept T>
	Ref<T> GetComponent(const UUID& entity) const {
		auto& map = GetComponentsMap<T>();
		auto it = map.find(entity);
		return (it != map.end()) ? it->second : nullptr;
	}

	std::vector<Ref<Component>> GetComponents(const UUID& entity) const {
		std::vector<Ref<Component>> result;

		for (auto& [type, storagePtr] : m_Storages) {
			const auto& baseMap = *static_cast<const std::unordered_map<UUID, Ref<Component>>*>(storagePtr);

			auto it = baseMap.find(entity);
			if (it != baseMap.end())
				result.push_back(it->second);
		}

		return result;
	}

	template <ComponentConcept T>
	std::vector<Ref<T>> GetAllComponentsOfType() const {
		auto& map = GetComponentsMap<T>();
		std::vector<Ref<T>> result;
		result.reserve(map.size());

		for (auto& [_, component] : map)
			result.push_back(component);

		return result;
	}

private:
	template <ComponentConcept T>
	static std::unordered_map<UUID, Ref<T>>& GetComponentsMap() {
		static std::unordered_map<UUID, Ref<T>> m_Storage;
		static bool registered = [] {
			m_Storages[typeid(T)] = &m_Storage;
			return true;
		}();

		return m_Storage;
	}

	inline static std::unordered_map<std::type_index, void*> m_Storages;

private:
};

}
