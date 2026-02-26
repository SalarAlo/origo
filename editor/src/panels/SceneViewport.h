#pragma once

#include "state/EditorContext.h"
#include <ImGuizmo.h>

#include "origo/assets/Texture2D.h"
#include "panels/EditorPanel.h"
#include "systems/EditorCamera.h"

namespace OrigoEditor {

class SceneViewport : public EditorPanel {
public:
	SceneViewport(EditorContext& ctx)
	    : m_context(ctx)
	    , m_camera(ctx.EditorViewportCamera) { }

	void on_im_gui_render();
	const char* get_name() const {
		return "Viewport";
	}

private:
	EditorContext& m_context;
	EditorCamera& m_camera;

	ImGuizmo::OPERATION m_gizmo_operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE m_gizmo_mode = ImGuizmo::LOCAL;

	Origo::Ref<Origo::Texture2D> m_move_icon;
	Origo::Ref<Origo::Texture2D> m_rotate_icon;
	Origo::Ref<Origo::Texture2D> m_scale_icon;

	bool m_icons_loaded = false;
};

}
