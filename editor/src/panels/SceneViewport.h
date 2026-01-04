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
	    : m_Context(ctx)
	    , m_Camera(ctx.EditorViewportCamera) { }

	void OnImGuiRender();
	const char* GetName() const {
		return "Viewport";
	}

private:
	EditorContext& m_Context;
	EditorCamera& m_Camera;

	ImGuizmo::OPERATION m_GizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE m_GizmoMode = ImGuizmo::LOCAL;

	Origo::Ref<Origo::Texture2D> m_MoveIcon;
	Origo::Ref<Origo::Texture2D> m_RotateIcon;
	Origo::Ref<Origo::Texture2D> m_ScaleIcon;

	bool m_IconsLoaded = false;
};

}
