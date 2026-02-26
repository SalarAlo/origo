#pragma once

#include <bits/utility.h>

#include <typeindex>
#include <unordered_map>

#include "ui/IInspectorDrawable.h"
#include "ui/InspectorDrawable.h"

namespace Origo {
class Texture2D;
}

namespace OrigoEditor {

class IInspectorDrawable;

struct InspectorEntry {
	const char* Name = nullptr;
	const char* IconPath = nullptr;
	Origo::Ref<Origo::Texture2D> Icon;
	Origo::Scope<IInspectorDrawable> Drawer;
};

class InspectorDrawRegistry {
	using ComponentToDrawerMap = std::unordered_map<std::type_index, InspectorEntry>;

public:
	template <typename T>
	static void register_native_drawer(const char* name, const char* iconPath, InspectorDrawable<T>::DrawFn fn) {
		InspectorEntry entry;
		entry.Name = name;
		entry.IconPath = iconPath;
		entry.Drawer = Origo::MakeScope<InspectorDrawable<T>>(name, std::move(fn));
		s_entries[typeid(T)] = std::move(entry);
	}

	static InspectorEntry* get(std::type_index type);
	static const ComponentToDrawerMap& get_entries() { return s_entries; }

	template <typename TComponent>
	static InspectorEntry* get() {
		return get(std::type_index(typeid(TComponent)));
	}

private:
	inline static ComponentToDrawerMap s_entries;
};
}
