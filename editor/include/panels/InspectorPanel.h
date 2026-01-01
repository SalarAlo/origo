#pragma once

#include "state/EditorContext.h"
#include "EditorPanel.h"

namespace OrigoEditor {

class InspectorPanel : public EditorPanel {
public:
	InspectorPanel(EditorContext& ctx)
	    : m_Context(ctx) { }

	const char* GetName() const override { return "Inspector"; }
	void OnImGuiRender() override;

private:
	void DrawEntityName();
	void DrawNativeComponents(Origo::Scene* scene, Origo::RID selectedEntity);
	void DrawScriptComponents(Origo::Scene* scene, Origo::RID selectedEntity);
	void DrawAddComponent(Origo::Scene* scene, Origo::RID selectedEntity);

private:
	EditorContext& m_Context;
};

}
