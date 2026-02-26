#pragma once

#include "state/EditorContext.h"
#include "EditorPanel.h"
#include "ui/InspectorComponentRenderer.h"

namespace OrigoEditor {

class EntityInspectorPanel : public EditorPanel {
public:
	EntityInspectorPanel(EditorContext& ctx)
	    : m_context(ctx) { InspectorComponentRenderer::init(&ctx); }

	const char* get_name() const override { return "Entity Inspector"; }
	void on_im_gui_render() override;

private:
	void draw_entity_name();
	void draw_native_components(Origo::Scene* scene, Origo::RID selectedEntity);
	void draw_script_components(Origo::Scene* scene, Origo::RID selectedEntity);
	void draw_add_component(Origo::Scene* scene, Origo::RID selectedEntity);
	void draw_script_drop_target(Origo::Scene* scene, Origo::RID selectedEntity);

private:
	EditorContext& m_context;
};

}
