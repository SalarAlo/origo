#pragma once

#include "InspectorDrawRegistry.h"

#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptFieldDescriptor.h"
#include "state/EditorContext.h"

namespace OrigoEditor {

class InspectorComponentRenderer {
public:
	static void Init(EditorContext* ctx);
	static void DrawNativeComponent(const Origo::RID& entity, void* componentPtr, std::type_index type);
	static void DrawScriptComponent(Origo::ScriptComponentInstance& instance);

private:
	static bool DrawHeaderNative(const Origo::RID& entity, void* id, const char* label, Origo::Ref<Origo::Texture2D> icon, std::type_index type);

	static bool DrawHeaderScripted(void* id, const char* label, Origo::Ref<Origo::Texture2D> icon);

	static void DrawUnknown(void* componentPtr);
	static void DrawScriptField(const Origo::ScriptFieldDescriptor& field, Origo::Variant& value);

	static Origo::Ref<Origo::Texture2D> LoadIcon(const std::string& path);

private:
	inline static Origo::Ref<Origo::Texture2D> s_UnknownIcon {};
	inline static Origo::Ref<Origo::Texture2D> s_ScriptIcon {};
	inline static EditorContext* s_Context {};
	inline static bool s_Initialised { false };
};

}
