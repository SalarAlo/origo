#pragma once

#include "origo/renderer/RenderView.h"
#include "state/EditorContext.h"

namespace OrigoEditor {

class EditorViewportController {
public:
	explicit EditorViewportController(EditorContext& ctx);

	Origo::RenderView GetActiveRenderView();

private:
	EditorContext& m_Context;
};

}
