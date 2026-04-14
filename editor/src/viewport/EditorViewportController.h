#pragma once

#include "origo/renderer/RenderView.h"

#include "state/EditorViewMode.h"

namespace Origo {
class Scene;
class CameraComponent;
}

namespace OrigoEditor {

class EditorViewportController {
public:
	explicit EditorViewportController(class EditorContext& ctx);
	Origo::RenderView get_render_view(EditorViewMode mode, Origo::Scene* scene);
	const Origo::CameraComponent* get_active_camera_component(EditorViewMode mode, Origo::Scene* scene) const;

private:
	EditorContext& m_context;
};

}
