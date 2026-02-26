#pragma once

#include "EditorPanel.h"
#include "state/EditorContext.h"
#include "systems/EditorRuntimeController.h"

namespace OrigoEditor {

class RuntimeStatePanel : public EditorPanel {
public:
	RuntimeStatePanel(EditorContext& context);
	const char* get_name() const override { return "RuntimePanel"; }

	void on_im_gui_render() override;

private:
	EditorContext& m_context;
	EditorRuntimeController m_controller;
};

}
