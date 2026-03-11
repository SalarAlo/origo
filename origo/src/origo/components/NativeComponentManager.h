#pragma once

#include <tuple>
#include <typeindex>

#include "origo/components/Component.h"

#include "origo/core/RID.h"

#include "origo/serialization/ISerializer.h"

namespace Origo {
struct NativeComponentTypeInfo;
}

namespace Origo {

template <typename T>
concept ComponentType = std::derived_from<T, Component>;

class NativeComponentManager {
public:
	using VisitFn = void (*)(const NativeComponentTypeInfo&, void* component, void* user);
	using VisitFnConst = void (*)(const NativeComponentTypeInfo&, const void* component, void* user);

public:
	NativeComponentManager() = default;
	NativeComponentManager(const NativeComponentManager& other);
	NativeComponentManager& operator=(const NativeComponentManager& other);

	static bool can_add_component_by_type(std::type_index type);

	bool add_component_by_type(const RID& entity, std::type_index type);
	bool remove_component_by_type(const RID& entity, std::type_index type);
	void remove_all_components(const RID& entity);

	bool has_component(const RID& entity, std::type_index type) const;
	void* get_component_by_type(const RID& entity, std::type_index type);
	const void* get_component_by_type(const RID& entity, std::type_index type) const;

	void for_each_component_on_entity(const RID& entity, VisitFn fn, void* user);
	void for_each_component_on_entity(const RID& entity, VisitFnConst fn, void* user) const;
	void copy_components(const RID& source, const RID& target);

	void serialize_entity(const RID& entity, ISerializer& backend) const;
	void deserialize_entity(const RID& entity, ISerializer& backend);

	template <ComponentType T, typename... Args>
	T& add_component(const RID& entity, Args&&... args) {
		auto& storage = get_or_create_storage<T>();
		auto [it, inserted] = storage.Data.try_emplace(entity, std::forward<Args>(args)...);
		(void)inserted;
		return it->second;
	}

	template <ComponentType T>
	bool has_component(const RID& entity) const {
		auto* storage = get_storage<T>();
		return storage && storage->has(entity);
	}

	template <ComponentType T>
	T* get_component(const RID& entity) {
		auto* storage = get_storage<T>();
		if (!storage)
			return nullptr;

		return storage->get(entity);
	}

	template <ComponentType T>
	const T* get_component(const RID& entity) const {
		auto* storage = get_storage<T>();

		if (!storage)
			return nullptr;

		return storage->get(entity);
	}

	template <ComponentType T, typename Func>
	void for_each(Func&& func) const {
		auto* storage = get_storage<T>();
		if (!storage)
			return;

		for (const auto& [entity, component] : storage->Data) {
			func(entity, component);
		}
	}

	template <ComponentType First, ComponentType... Rest, typename Func>
	void view(Func&& func) {
		auto* primary = get_storage<First>();
		if (!primary)
			return;

		if constexpr (sizeof...(Rest) > 0) {
			auto other_storages = std::tuple<Storage<Rest>*...> { get_storage<Rest>()... };
			if (!std::apply(
			        [](auto*... storages) { return ((storages != nullptr) && ...); },
			        other_storages)) {
				return;
			}

			for (auto& [entity, first] : primary->Data) {
				auto rest_ptrs = std::apply(
				    [&](auto*... storages) {
					    return std::tuple<Rest*...> { storages->get(entity)... };
				    },
				    other_storages);

				if (!std::apply(
				        [](auto*... ptrs) { return ((ptrs != nullptr) && ...); },
				        rest_ptrs)) {
					continue;
				}

				std::apply(
				    [&](auto*... ptrs) {
					    func(entity, first, *ptrs...);
				    },
				    rest_ptrs);
			}
		} else {
			for (auto& [entity, first] : primary->Data) {
				func(entity, first);
			}
		}
	}

	template <ComponentType First, ComponentType... Rest, typename Func>
	void view(Func&& func) const {
		auto* primary = get_storage<First>();
		if (!primary)
			return;

		if constexpr (sizeof...(Rest) > 0) {
			auto other_storages = std::tuple<const Storage<Rest>*...> { get_storage<Rest>()... };
			if (!std::apply(
			        [](const auto*... storages) { return ((storages != nullptr) && ...); },
			        other_storages)) {
				return;
			}

			for (const auto& [entity, first] : primary->Data) {
				auto rest_ptrs = std::apply(
				    [&](const auto*... storages) {
					    return std::tuple<const Rest*...> { storages->get(entity)... };
				    },
				    other_storages);

				if (!std::apply(
				        [](const auto*... ptrs) { return ((ptrs != nullptr) && ...); },
				        rest_ptrs)) {
					continue;
				}

				std::apply(
				    [&](const auto*... ptrs) {
					    func(entity, first, *ptrs...);
				    },
				    rest_ptrs);
			}
		} else {
			for (const auto& [entity, first] : primary->Data) {
				func(entity, first);
			}
		}
	}

	template <typename Func>
	void for_each_component_on_entity(const RID& entity, Func&& func) {
		auto thunk = [](const NativeComponentTypeInfo& info, void* c, void* u) {
			(*static_cast<std::remove_reference_t<Func>*>(u))(info, c);
		};
		this->for_each_component_on_entity(entity, thunk, &func);
	}

	template <typename Func>
	void for_each_component_on_entity(const RID& entity, Func&& func) const {
		auto thunk = [](const NativeComponentTypeInfo& info, const void* c, void* u) {
			(*static_cast<std::remove_reference_t<Func>*>(u))(info, c);
		};
		this->for_each_component_on_entity(entity, thunk, &func);
	}

private:
	void clone_from(const NativeComponentManager& other);

private:
	struct IStorage {
		virtual ~IStorage() = default;
		virtual bool has(const RID& entity) const = 0;
		virtual void* get_raw(const RID& entity) = 0;
		virtual const void* get_raw(const RID& entity) const = 0;
		virtual bool remove(const RID& entity) = 0;
		virtual std::unique_ptr<IStorage> clone() const = 0;
	};

	template <ComponentType T>
	struct Storage final : IStorage {
		std::unordered_map<RID, T> Data;

		bool has(const RID& entity) const override {
			return Data.find(entity) != Data.end();
		}

		T* get(const RID& entity) {
			auto it = Data.find(entity);
			return it != Data.end() ? &it->second : nullptr;
		}

		const T* get(const RID& entity) const {
			auto it = Data.find(entity);
			return it != Data.end() ? &it->second : nullptr;
		}

		void* get_raw(const RID& entity) override {
			return get(entity);
		}

		const void* get_raw(const RID& entity) const override {
			return get(entity);
		}

		bool remove(const RID& entity) override {
			return Data.erase(entity) > 0;
		}

		std::unique_ptr<IStorage> clone() const override {
			auto cloned = std::make_unique<Storage<T>>();
			cloned->Data = Data;
			return cloned;
		}
	};

private:
	template <ComponentType T>
	Storage<T>& get_or_create_storage() {
		const std::type_index type = typeid(T);

		auto it = m_storages.find(type);
		if (it == m_storages.end()) {
			auto storage = std::make_unique<Storage<T>>();
			auto* ptr = storage.get();
			m_storages.emplace(type, std::move(storage));
			return *ptr;
		}

		return *static_cast<Storage<T>*>(it->second.get());
	}

	template <ComponentType T>
	Storage<T>* get_storage() {
		const std::type_index type = typeid(T);
		auto it = m_storages.find(type);
		return it != m_storages.end()
		    ? static_cast<Storage<T>*>(it->second.get())
		    : nullptr;
	}

	template <ComponentType T>
	const Storage<T>* get_storage() const {
		const std::type_index type = typeid(T);
		auto it = m_storages.find(type);
		return it != m_storages.end()
		    ? static_cast<const Storage<T>*>(it->second.get())
		    : nullptr;
	}

private:
	std::unordered_map<std::type_index, Scope<IStorage>> m_storages;
};

}
