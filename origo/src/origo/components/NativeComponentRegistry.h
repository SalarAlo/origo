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
	static void Register(const std::string& name, IComponentSerializer* serializer) {
		const std::type_index type = typeid(T);

		auto& map = GetMap();
		if (map.contains(type))
			throw std::runtime_error("Component already registered");

		map.emplace(type,
		    NativeComponentTypeInfo {
		        .DisplayName = name,
		        .Type = type,
		        .Add = [](NativeComponentManager& mgr, RID e) { mgr.AddComponent<T>(e); },
		        .Remove = [](NativeComponentManager& mgr, RID e) { mgr.RemoveComponentByType(e, typeid(T)); },
		        .Has = [](const NativeComponentManager& mgr, RID e) { return mgr.HasComponent<T>(e); },
		        .Get = [](NativeComponentManager& mgr, RID e) -> void* { return mgr.GetComponent<T>(e); },
		        .Serializer = serializer });

		auto& nameMap = GetNameMap();
		if (nameMap.contains(name))
			throw std::runtime_error("Component with name '" + name + "' already registered");
		nameMap.emplace(name, type);
	}

	static std::unordered_map<std::string, std::type_index>& GetNameMap() {
		static std::unordered_map<std::string, std::type_index> map;
		return map;
	}

	static const NativeComponentTypeInfo* GetByName(std::string_view name) {
		auto& nameMap = GetNameMap();
		auto it = nameMap.find(std::string(name));
		if (it == nameMap.end())
			return nullptr;
		return Get(it->second);
	}

	static const auto& GetAll() { return GetMap(); }

	static const NativeComponentTypeInfo* Get(std::type_index type) {
		auto& map = GetMap();
		auto it = map.find(type);
		return it != map.end() ? &it->second : nullptr;
	}

private:
	static std::unordered_map<std::type_index, NativeComponentTypeInfo>& GetMap() {
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
		::Origo::NativeComponentRegistry::Register<T>(            \
		    T {}.GetComponentName(),                              \
		    &ORIGO_DETAIL_CONCAT(_origo_serializer_, N));         \
		return true;                                              \
	}();

#define ORIGO_REGISTER_NATIVE_COMPONENT(T, SerializerType) \
	ORIGO_REGISTER_NATIVE_COMPONENT_IMPL(T, SerializerType, __COUNTER__)

#define ORIGO_REGISTER_NATIVE_COMPONENT_NO_SERIALIZE_IMPL(T, N) \
	static bool ORIGO_DETAIL_CONCAT(_origo_reg_, N) = [] {  \
		::Origo::NativeComponentRegistry::Register<T>(  \
		    T {}.GetComponentName(),                    \
		    nullptr);                                   \
		return true;                                    \
	}();

#define ORIGO_REGISTER_NATIVE_COMPONENT_NO_SERIALIZE(T) \
	ORIGO_REGISTER_NATIVE_COMPONENT_NO_SERIALIZE_IMPL(T, __COUNTER__)
