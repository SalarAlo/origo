#include "viewport/EditorViewportController.h"
#include "origo/scene/CameraComponent.h"

namespace OrigoEditor {

EditorViewportController::EditorViewportController(EditorContext& ctx)
    : m_Context(ctx) { }

Origo::Camera* EditorViewportController::GetActiveCamera() {
	if (m_Context.RuntimeState == EditorRuntimeState::Running) {
		return &m_Context.EditorViewportCamera.GetCamera();
	} else if (m_Context.RuntimeState == EditorRuntimeState::Running) {
		Origo::CameraComponent* primary {};
		m_Context.RuntimeScene->View<Origo::CameraComponent>(
		    [&](Origo::RID entity,
		        Origo::CameraComponent& cp) {
			    if (primary)
				    return;
			    if (cp.IsPrimary)
				    primary = &cp;
		    });
		return &primary->GetCamera();
	}

	return nullptr;
}
}
