#pragma once

#include "InspectorDrawable.h"
#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptFieldDescriptor.h"
#include "ui/IInspectorDrawable.h"

#include <typeindex>
#include <unordered_map>
#include <string>

namespace Origo {
class Texture2D;
}

namespace OrigoEditor {

class InspectorDrawRegistry {
private:
	struct InspectorEntry {
		const char* Name = nullptr;
		const char* IconPath = nullptr;
		Origo::Ref<Origo::Texture2D> Icon;
		Origo::Scope<IInspectorDrawable> Drawer;
	};

public:
	template <typename T>
	static void RegisterNativeDrawer(
	    const char* name,
	    const char* iconPath,
	    InspectorDrawable<T>::DrawFn fn) {
		InspectorEntry entry;
		entry.Name = name;
		entry.IconPath = iconPath;
		entry.Drawer = Origo::MakeScope<InspectorDrawable<T>>(name, std::move(fn));
		m_Drawers[typeid(T)] = std::move(entry);
	}

	static void DrawScriptComponent(Origo::ScriptComponentInstance& instance);
	static void DrawNativeComponent(void* drawablePtr, std::type_index type);

	static const auto& GetEntries() { return m_Drawers; }

private:
	static bool DrawScriptComponentHeader(const char* name, void* idPtr);

	static bool DrawInspectorHeaderRow(
	    void* idPtr,
	    ImTextureID icon,
	    const char* label,
	    float iconSize = 18.0f,
	    float iconCellW = 22.0f);

	static void DrawScriptField(
	    const Origo::ScriptFieldDescriptor& field,
	    Origo::Variant& value);

	static void DrawUnknown(void* drawablePtr);
	static void DrawKnown(void* drawablePtr, InspectorEntry& entry);

	static void EnsureUnknownIcon();
	static void EnsureScriptIcon();
	static void EnsureEntryIcon(InspectorEntry& entry);

	static Origo::Ref<Origo::Texture2D> LoadIcon(const std::string& path);

private:
	inline static std::unordered_map<std::type_index, InspectorEntry> m_Drawers;
	inline static Origo::Ref<Origo::Texture2D> m_UnknownIcon;
	inline static Origo::Ref<Origo::Texture2D> m_ScriptIcon;
};

}
