#pragma once

#include <typeindex>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include "origo/core/RID.h"
#include "origo/scene/Component.h"

namespace Origo {

template <typename T>
concept ComponentType = std::derived_from<T, Component>;

class ComponentManager {
private:
	struct IStorage {
		virtual ~IStorage() = default;
		virtual bool Has(RID entity) const = 0;
		virtual void* GetRaw(RID entity) = 0;
		virtual const void* GetRaw(RID entity) const = 0;
		virtual std::unique_ptr<IStorage> Clone() const = 0;
	};

	template <ComponentType T>
	struct Storage final : IStorage {
		std::unordered_map<RID, T> Data;

		bool Has(RID entity) const override {
			return Data.find(entity) != Data.end();
		}

		void* GetRaw(RID entity) override {
			auto it = Data.find(entity);
			return it != Data.end() ? &it->second : nullptr;
		}

		const void* GetRaw(RID entity) const override {
			auto it = Data.find(entity);
			return it != Data.end() ? &it->second : nullptr;
		}

		std::unique_ptr<IStorage> Clone() const override {
			auto cloned = std::make_unique<Storage<T>>();
			cloned->Data = Data;
			return cloned;
		}
	};

public:
	ComponentManager() = default;
	ComponentManager(const ComponentManager& other);
	ComponentManager& operator=(const ComponentManager& other);

	void CloneFrom(const ComponentManager& other);
	bool AddComponentByType(RID entity, std::type_index type);
	static bool CanAddComponentByType(std::type_index type);

	template <ComponentType T, typename... Args>
	T& AddComponent(RID entity, Args&&... args) {
		auto& storage = GetOrCreateStorage<T>();
		auto [it, inserted] = storage.Data.emplace(entity, T { std::forward<Args>(args)... });

		if (!inserted)
			throw std::runtime_error("Component already exists on entity");

		return it->second;
	}

	template <ComponentType T>
	bool HasComponent(RID entity) const {
		auto* storage = GetStorage<T>();
		return storage && storage->Has(entity);
	}

	template <ComponentType T>
	T* GetComponent(RID entity) {
		auto* storage = GetStorage<T>();
		if (!storage)
			return nullptr;

		auto it = storage->Data.find(entity);
		return it != storage->Data.end() ? &it->second : nullptr;
	}

	template <ComponentType T>
	const T* GetComponent(RID entity) const {
		auto* storage = GetStorage<T>();
		if (!storage)
			return nullptr;

		auto it = storage->Data.find(entity);
		return it != storage->Data.end() ? &it->second : nullptr;
	}

	template <ComponentType T, typename Func>
	void ForEach(Func&& func) const {
		auto* storage = GetStorage<T>();
		if (!storage)
			return;

		for (const auto& [entity, component] : storage->Data) {
			func(entity, component);
		}
	}

	template <ComponentType First, ComponentType... Rest, typename Func>
	void View(Func&& func) {
		auto* primary = GetStorage<First>();
		if (!primary)
			return;

		for (auto& [entity, first] : primary->Data) {
			if ((HasComponent<Rest>(entity) && ...)) {
				func(entity, first, *GetComponent<Rest>(entity)...);
			}
		}
	}

	template <ComponentType First, ComponentType... Rest, typename Func>
	void View(Func&& func) const {
		auto* primary = GetStorage<First>();
		if (!primary)
			return;

		for (const auto& [entity, first] : primary->Data) {
			if ((HasComponent<Rest>(entity) && ...)) {
				func(entity, first, *GetComponent<Rest>(entity)...);
			}
		}
	}

	bool HasComponent(RID entity, std::type_index type) const {
		auto it = m_Storages.find(type);
		return it != m_Storages.end() && it->second->Has(entity);
	}

	void* GetComponentByType(RID entity, std::type_index type) {
		auto it = m_Storages.find(type);
		return it != m_Storages.end() ? it->second->GetRaw(entity) : nullptr;
	}

	const void* GetComponentByType(RID entity, std::type_index type) const {
		auto it = m_Storages.find(type);
		return it != m_Storages.end() ? it->second->GetRaw(entity) : nullptr;
	}

private:
	template <ComponentType T>
	Storage<T>& GetOrCreateStorage() {
		const std::type_index type = typeid(T);

		auto it = m_Storages.find(type);
		if (it == m_Storages.end()) {
			auto storage = std::make_unique<Storage<T>>();
			auto* ptr = storage.get();
			m_Storages.emplace(type, std::move(storage));
			return *ptr;
		}

		return *static_cast<Storage<T>*>(it->second.get());
	}

	template <ComponentType T>
	Storage<T>* GetStorage() {
		const std::type_index type = typeid(T);
		auto it = m_Storages.find(type);
		return it != m_Storages.end()
		    ? static_cast<Storage<T>*>(it->second.get())
		    : nullptr;
	}

	template <ComponentType T>
	const Storage<T>* GetStorage() const {
		const std::type_index type = typeid(T);
		auto it = m_Storages.find(type);
		return it != m_Storages.end()
		    ? static_cast<const Storage<T>*>(it->second.get())
		    : nullptr;
	}

private:
	std::unordered_map<std::type_index, std::unique_ptr<IStorage>> m_Storages;
};

}
