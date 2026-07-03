#pragma once

#include "InspectorDrawRegistry.h"

#include "origo/scripting/ComponentDataManager.h"
#include "origo/scripting/ScriptAttachmentManager.h"

#include "origo/utils/Singleton.h"

#include "state/EditorContext.h"

namespace OrigoEditor {

class InspectorComponentRenderer : public Origo::Singleton<InspectorComponentRenderer> {
public:
	void init(EditorContext* ctx);
	void draw_native_component(const Origo::RID& entity, void* componentPtr, std::type_index type);
	void draw_reflected_component(const Origo::RID& entity, Origo::ComponentInstanceData& instance);
	void draw_reflected_script(const Origo::RID& entity, Origo::ScriptAttachmentData& attachment);

private:
	bool draw_header_native(const Origo::RID& entity, void* id, const char* label, Origo::Ref<Origo::Texture2D> icon, std::type_index type);
	bool draw_header_reflected(const Origo::RID& entity, void* id, const char* label, Origo::Ref<Origo::Texture2D> icon, bool behavior_script, bool removable);
	void draw_unknown(void* componentPtr);
	void draw_reflected_field(Origo::ReflectedFieldValue& field);
	void draw_entity_ref_field(Origo::ReflectedFieldValue& field);
	Origo::Ref<Origo::Texture2D> load_icon(const std::string& path);

private:
	Origo::Ref<Origo::Texture2D> m_unknown_icon {};
	Origo::Ref<Origo::Texture2D> m_script_icon {};
	EditorContext* m_context {};
	bool m_initialised { false };
};

}
