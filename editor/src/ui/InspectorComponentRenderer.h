#pragma once

#include "InspectorDrawRegistry.h"

#include "origo/scripting/ScriptComponentInstance.h"
#include "origo/scripting/ScriptFieldDescriptor.h"

#include "origo/utils/Singleton.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class InspectorComponentRenderer : public Origo::Singleton<InspectorComponentRenderer> {
public:
	void init(EditorContext* ctx);
	void draw_native_component(const Origo::RID& entity, void* componentPtr, std::type_index type);
	void draw_script_component(Origo::ScriptComponentInstance& instance);

private:
	bool draw_header_native(const Origo::RID& entity, void* id, const char* label, Origo::Ref<Origo::Texture2D> icon, std::type_index type);
	bool draw_header_scripted(void* id, const char* label, Origo::Ref<Origo::Texture2D> icon);
	void draw_unknown(void* componentPtr);
	void draw_script_field(const Origo::ScriptFieldDescriptor& field, Origo::Variant& value);
	Origo::Ref<Origo::Texture2D> load_icon(const std::string& path);

private:
	Origo::Ref<Origo::Texture2D> m_unknown_icon {};
	Origo::Ref<Origo::Texture2D> m_script_icon {};
	EditorContext* m_context {};
	bool m_initialised { false };
};

}
