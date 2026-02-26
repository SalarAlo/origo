#pragma once

#include "InspectorDrawRegistry.h"

#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptFieldDescriptor.h"
#include "state/EditorContext.h"

namespace OrigoEditor {

class InspectorComponentRenderer {
public:
	static void init(EditorContext* ctx);
	static void draw_native_component(const Origo::RID& entity, void* componentPtr, std::type_index type);
	static void draw_script_component(Origo::ScriptComponentInstance& instance);

private:
	static bool draw_header_native(const Origo::RID& entity, void* id, const char* label, Origo::Ref<Origo::Texture2D> icon, std::type_index type);

	static bool draw_header_scripted(void* id, const char* label, Origo::Ref<Origo::Texture2D> icon);

	static void draw_unknown(void* componentPtr);
	static void draw_script_field(const Origo::ScriptFieldDescriptor& field, Origo::Variant& value);

	static Origo::Ref<Origo::Texture2D> load_icon(const std::string& path);

private:
	inline static Origo::Ref<Origo::Texture2D> s_unknown_icon {};
	inline static Origo::Ref<Origo::Texture2D> s_script_icon {};
	inline static EditorContext* s_context {};
	inline static bool s_initialised { false };
};

}
