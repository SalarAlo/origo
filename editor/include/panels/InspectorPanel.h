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
	EditorContext& m_Context;
};

}
