#pragma once

#include "NativeComponentManager.h"

#include "origo/components/serialization/IComponentSerializer.h"

namespace Origo {

struct NativeComponentTypeInfo {
	std::string DisplayName;
	std::type_index Type;

	void (*Add)(NativeComponentManager&, RID);
	void (*Remove)(NativeComponentManager&, RID);
	bool (*Has)(const NativeComponentManager&, RID);
	void* (*Get)(NativeComponentManager&, RID);

	IComponentSerializer* Serializer;
};
class NativeComponentRegistry {
public:
	template <typename T>
	static void register_component(const std::string& name, IComponentSerializer* serializer) {
		const std::type_index type = typeid(T);

		auto& map = get_map();
		if (map.contains(type))
			throw std::runtime_error("Component already registered");

		map.emplace(type,
		    NativeComponentTypeInfo {
		        .DisplayName = name,
		        .Type = type,
		        .Add = [](NativeComponentManager& mgr, RID e) { mgr.add_component<T>(e); },
		        .Remove = [](NativeComponentManager& mgr, RID e) { mgr.remove_component_by_type(e, typeid(T)); },
		        .Has = [](const NativeComponentManager& mgr, RID e) { return mgr.has_component<T>(e); },
		        .Get = [](NativeComponentManager& mgr, RID e) -> void* { return mgr.get_component<T>(e); },
		        .Serializer = serializer });

		auto& name_map = get_name_map();
		if (name_map.contains(name))
			throw std::runtime_error("Component with name '" + name + "' already registered");
		name_map.emplace(name, type);
	}

	static std::unordered_map<std::string, std::type_index>& get_name_map() {
		static std::unordered_map<std::string, std::type_index> map;
		return map;
	}

	static const NativeComponentTypeInfo* get_by_name(std::string_view name) {
		auto& name_map = get_name_map();
		auto it = name_map.find(std::string(name));
		if (it == name_map.end())
			return nullptr;
		return get(it->second);
	}

	static const auto& get_all() { return get_map(); }

	static const NativeComponentTypeInfo* get(std::type_index type) {
		auto& map = get_map();
		auto it = map.find(type);
		return it != map.end() ? &it->second : nullptr;
	}

private:
	static std::unordered_map<std::type_index, NativeComponentTypeInfo>& get_map() {
		static std::unordered_map<std::type_index, NativeComponentTypeInfo> map;
		return map;
	}
};
}

#define ORIGO_DETAIL_CONCAT_IMPL(a, b) a##b
#define ORIGO_DETAIL_CONCAT(a, b) ORIGO_DETAIL_CONCAT_IMPL(a, b)

#define ORIGO_REGISTER_NATIVE_COMPONENT_IMPL(T, SerializerType, N)        \
	static SerializerType ORIGO_DETAIL_CONCAT(_origo_serializer_, N); \
	static bool ORIGO_DETAIL_CONCAT(_origo_reg_, N) = [] {            \
		::Origo::NativeComponentRegistry::register_component<T>(  \
		    T {}.get_component_name(),                            \
		    &ORIGO_DETAIL_CONCAT(_origo_serializer_, N));         \
		return true;                                              \
	}();

#define ORIGO_REGISTER_NATIVE_COMPONENT(T, SerializerType) \
	ORIGO_REGISTER_NATIVE_COMPONENT_IMPL(T, SerializerType, __COUNTER__)

#define ORIGO_REGISTER_NATIVE_COMPONENT_NO_SERIALIZE_IMPL(T, N)          \
	static bool ORIGO_DETAIL_CONCAT(_origo_reg_, N) = [] {           \
		::Origo::NativeComponentRegistry::register_component<T>( \
		    T {}.get_component_name(),                           \
		    nullptr);                                            \
		return true;                                             \
	}();

#define ORIGO_REGISTER_NATIVE_COMPONENT_NO_SERIALIZE(T) \
	ORIGO_REGISTER_NATIVE_COMPONENT_NO_SERIALIZE_IMPL(T, __COUNTER__)
