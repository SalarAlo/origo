#pragma once

#include "state/EditorContext.h"
#include "EditorPanel.h"

namespace OrigoEditor {

class InspectorPanel : public EditorPanel {
public:
	InspectorPanel(EditorContext& ctx)
	    : m_Contex(ctx) { }

	const char* GetName() const override { return "Inspector"; }
	void OnImGuiRender() override;

private:
	EditorContext& m_Contex;
};

}
