#pragma once

#include "origo/renderer/RenderView.h"

namespace OrigoEditor {

class EditorViewportController {
public:
	explicit EditorViewportController(class EditorContext& ctx);
	Origo::RenderView GetActiveRenderView();

private:
	EditorContext& m_Context;
};

}
