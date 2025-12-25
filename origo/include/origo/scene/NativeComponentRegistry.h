#pragma once

#include "origo/scene/NativeComponentManager.h"
namespace Origo {

struct NativeComponentTypeInfo {
	std::string DisplayName;
	std::type_index Type;
	void (*Add)(NativeComponentManager&, RID);
	void (*Remove)(NativeComponentManager&, RID);
	bool (*Has)(const NativeComponentManager&, RID);
	void* (*Get)(NativeComponentManager&, RID);
};

class NativeComponentRegistry {
public:
	template <typename T>
	static void Register(const std::string& name) {
		const std::type_index type = typeid(T);

		auto& map = GetMap();
		if (map.contains(type))
			throw std::runtime_error("Component already registered");

		map.emplace(type,
		    NativeComponentTypeInfo {
		        .DisplayName = name,
		        .Type = type,
		        .Add = [](NativeComponentManager& mgr, RID e) { mgr.AddComponent<T>(e); },
		        .Remove = [](NativeComponentManager& mgr, RID e) { /* optional */ },
		        .Has = [](const NativeComponentManager& mgr, RID e) { return mgr.HasComponent<T>(e); },
		        .Get = [](NativeComponentManager& mgr, RID e) -> void* { return mgr.GetComponent<T>(e); } });
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

#define ORIGO_REGISTER_NATIVE_COMPONENT(T)                                     \
	static bool ORIGO_DETAIL_CONCAT(_origo_reg_, __COUNTER__) = [] {       \
		::Origo::NativeComponentRegistry::Register<T>(T {}.GetName()); \
		return true;                                                   \
	}();
