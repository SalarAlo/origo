#pragma once

#include "EditorContext.h"
#include <ImGuizmo.h>

#include "origo/Camera.h"
#include "origo/assets/Texture.h"
#include "panels/EditorPanel.h"

namespace OrigoEditor {

class SceneViewport : public EditorPanel {
public:
	SceneViewport(EditorContext& ctx)
	    : m_Context(ctx)
	    , m_Camera(ctx.Scene.GetMainCamera()) { }

	void OnImGuiRender();
	const char* GetName() const { return "Viewport"; }

private:
	EditorContext& m_Context;
	Origo::Camera* m_Camera;

	ImGuizmo::OPERATION m_GizmoOperation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE m_GizmoMode = ImGuizmo::LOCAL;

	Origo::Ref<Origo::Texture> m_MoveIcon;
	Origo::Ref<Origo::Texture> m_RotateIcon;
	Origo::Ref<Origo::Texture> m_ScaleIcon;

	bool m_IconsLoaded = false;
};

}
