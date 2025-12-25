#pragma once

#include "origo/scene/ComponentManager.h"
namespace Origo {

struct ComponentTypeInfo {
	std::string DisplayName;
	std::type_index Type;
	void (*Add)(ComponentManager&, RID);
	void (*Remove)(ComponentManager&, RID);
	bool (*Has)(const ComponentManager&, RID);
	void* (*Get)(ComponentManager&, RID);
};

class ComponentRegistry {
public:
	template <typename T>
	static void Register(const std::string& name) {
		const std::type_index type = typeid(T);

		auto& map = GetMap();
		if (map.contains(type))
			throw std::runtime_error("Component already registered");

		map.emplace(type,
		    ComponentTypeInfo {
		        .DisplayName = name,
		        .Type = type,
		        .Add = [](ComponentManager& mgr, RID e) { mgr.AddComponent<T>(e); },
		        .Remove = [](ComponentManager& mgr, RID e) { /* optional */ },
		        .Has = [](const ComponentManager& mgr, RID e) { return mgr.HasComponent<T>(e); },
		        .Get = [](ComponentManager& mgr, RID e) -> void* { return mgr.GetComponent<T>(e); } });
	}

	static const auto& GetAll() { return GetMap(); }

	static const ComponentTypeInfo* Get(std::type_index type) {
		auto& map = GetMap();
		auto it = map.find(type);
		return it != map.end() ? &it->second : nullptr;
	}

private:
	static std::unordered_map<std::type_index, ComponentTypeInfo>& GetMap() {
		static std::unordered_map<std::type_index, ComponentTypeInfo> map;
		return map;
	}
};
}

#define ORIGO_DETAIL_CONCAT_IMPL(a, b) a##b
#define ORIGO_DETAIL_CONCAT(a, b) ORIGO_DETAIL_CONCAT_IMPL(a, b)

#define ORIGO_REGISTER_COMPONENT(T)                                      \
	static bool ORIGO_DETAIL_CONCAT(_origo_reg_, __COUNTER__) = [] { \
		::Origo::ComponentRegistry::Register<T>(T {}.GetName()); \
		return true;                                             \
	}();
