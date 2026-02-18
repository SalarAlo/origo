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
	static void RegisterNativeDrawer(const char* name, const char* iconPath, InspectorDrawable<T>::DrawFn fn) {
		InspectorEntry entry;
		entry.Name = name;
		entry.IconPath = iconPath;
		entry.Drawer = Origo::MakeScope<InspectorDrawable<T>>(name, std::move(fn));
		s_Entries[typeid(T)] = std::move(entry);
	}

	static InspectorEntry* Get(std::type_index type);
	static const ComponentToDrawerMap& GetEntries() { return s_Entries; }

	template <typename TComponent>
	static InspectorEntry* Get() {
		return Get(std::type_index(typeid(TComponent)));
	}

private:
	inline static ComponentToDrawerMap s_Entries;
};
}
