#pragma once

#include <bits/utility.h>

#include <typeindex>
#include <unordered_map>

#include "origo/utils/Singleton.h"

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

class InspectorDrawRegistry : public Origo::Singleton<InspectorDrawRegistry> {
	using ComponentToDrawerMap = std::unordered_map<std::type_index, InspectorEntry>;

public:
	template <typename T>
	void register_native_drawer(const char* name, const char* iconPath, InspectorDrawable<T>::DrawFn fn) {
		InspectorEntry entry;
		entry.Name = name;
		entry.IconPath = iconPath;
		entry.Drawer = Origo::make_scope<InspectorDrawable<T>>(name, std::move(fn));
		m_entries[typeid(T)] = std::move(entry);
	}

	InspectorEntry* get_inspector_entry(std::type_index type);
	const ComponentToDrawerMap& get_entries() { return m_entries; }

	template <typename TComponent>
	InspectorEntry* get_inspector_entry() {
		return get_inspector_entry(std::type_index(typeid(TComponent)));
	}

private:
	ComponentToDrawerMap m_entries {};
};
}
