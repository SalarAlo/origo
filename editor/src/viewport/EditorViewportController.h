#pragma once

#include "origo/renderer/RenderView.h"

namespace OrigoEditor {

class EditorViewportController {
public:
	explicit EditorViewportController(class EditorContext& ctx);
	Origo::RenderView get_and_update_active_render_view();

private:
	EditorContext& m_context;
};

}
