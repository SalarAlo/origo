#pragma once

#include "origo/core/RID.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"
#include <concepts>
#include <typeindex>
#include <unordered_map>

namespace Origo {

template <typename T>
concept ComponentType = std::derived_from<T, Component>;

class ComponentManager {
	using ComponentsMap = std::unordered_map<RID, Component*>;

public:
	~ComponentManager() {
		for (const auto& deleter : m_Deleters)
			deleter();
	}

	template <ComponentType T, typename... Args>
	T* AddComponent(Entity* entity, Args&&... args) {
		auto& map = GetComponentsMap<T>();
		auto component = new T(entity, std::forward<Args>(args)...);
		map[entity->GetId()] = component;

		return component;
	}

	template <ComponentType T>
	T* GetComponent(const RID& entity) const {
		auto& map = GetComponentsMap<T>();
		auto it = map.find(entity);
		return (it != map.end()) ? it->second : nullptr;
	}

	std::vector<Component*> GetComponents(const RID& entity) const {
		std::vector<Component*> result;

		for (auto& [type, storagePtr] : m_Storages) {
			const auto& baseMap = *static_cast<const ComponentsMap*>(storagePtr);

			auto it = baseMap.find(entity);

			if (it != baseMap.end())
				result.push_back(it->second);
		}

		return result;
	}

	template <ComponentType T>
	std::vector<T*> GetAllComponentsOfType() const {
		auto& map = GetComponentsMap<T>();
		std::vector<T*> result;
		result.reserve(map.size());

		for (auto& [_, component] : map)
			result.push_back(component);

		return result;
	}

private:
	template <ComponentType T>
	static std::unordered_map<RID, T*>& GetComponentsMap() {
		static std::unordered_map<RID, T*> m_Storage;
		static bool _ = [] {
			m_Storages[typeid(T)] = &m_Storage;
			m_Deleters.push_back([] {
				for (const auto& [_, ptr] : m_Storage)
					delete static_cast<T*>(ptr);
				m_Storage.clear();
			});

			return true;
		}();

		return m_Storage;
	}

	inline static std::unordered_map<std::type_index, void*> m_Storages;
	inline static std::vector<std::function<void()>> m_Deleters;

private:
};

}
