#include "viewport/EditorViewportController.h"
#include "origo/renderer/RenderView.h"
#include "origo/scene/CameraComponent.h"
#include "origo/scene/Transform.h"
#include "state/EditorRuntimeState.h"

using namespace Origo;
namespace OrigoEditor {

EditorViewportController::EditorViewportController(EditorContext& ctx)
    : m_Context(ctx) { }

RenderView EditorViewportController::GetActiveRenderView() {
	Camera* cam { nullptr };
	Transform* transf { nullptr };

	if (m_Context.RuntimeState == EditorRuntimeState::Running) {
		m_Context.ActiveScene->View<CameraComponent, Transform>([&](RID e, CameraComponent& cc, Transform& tr) {
			if (cam != nullptr)
				return;

			if (cc.IsPrimary) {
				cam = &cc.GetCamera();
				cam->UpdateFromTransform(tr);
				transf = &tr;
			}
		});
	}

	if (m_Context.RuntimeState == EditorRuntimeState::EditingOnly || !cam) {
		cam = &m_Context.EditorViewportCamera.GetCamera();
		transf = &m_Context.EditorViewportCamera.GetTransform();
	}

	cam->SetAspect(
	    static_cast<float>(m_Context.RenderBuffer.GetWidth())
	    / static_cast<float>(m_Context.RenderBuffer.GetHeight()));

	RenderView view {
		.Projection = cam->GetProjection(),
		.View = cam->GetView(),
		.CameraForward = transf->GetForward(),
		.CameraPosition = transf->GetPosition(),
	};

	return view;
}
}
