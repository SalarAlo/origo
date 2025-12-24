#pragma once

#include "EditorPanel.h"
#include "state/EditorContext.h"
#include "systems/EditorRuntimeController.h"

namespace OrigoEditor {

class RuntimeStatePanel : public EditorPanel {
public:
	RuntimeStatePanel(EditorContext& context)
	    : m_Context(context)
	    , m_Controller(context) { }
	const char* GetName() const override { return "RuntimePanel"; }

	void OnImGuiRender() override;

private:
	EditorContext& m_Context;
	EditorRuntimeController m_Controller;
};

}
