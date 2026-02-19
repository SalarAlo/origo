#pragma once

#include "origo/renderer/RenderView.h"

namespace OrigoEditor {

class EditorViewportController {
public:
	explicit EditorViewportController(class EditorContext& ctx);
	Origo::RenderView GetAndUpdateActiveRenderView();

private:
	EditorContext& m_Context;
};

}
