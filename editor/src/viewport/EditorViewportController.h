#pragma once

#include "origo/renderer/RenderView.h"
#include "state/EditorViewMode.h"

namespace Origo {
class Scene;
}

namespace OrigoEditor {

class EditorViewportController {
public:
	explicit EditorViewportController(class EditorContext& ctx);
	Origo::RenderView get_render_view(EditorViewMode mode, Origo::Scene* scene);

private:
	EditorContext& m_context;
};

}
