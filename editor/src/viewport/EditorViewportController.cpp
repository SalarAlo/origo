#include "viewport/EditorViewportController.h"
#include "origo/renderer/RenderView.h"
#include "origo/components/CameraComponent.h"
#include "origo/components/Transform.h"
#include "state/EditorViewMode.h"
#include "state/EditorContext.h"

using namespace Origo;
namespace OrigoEditor {

EditorViewportController::EditorViewportController(EditorContext& ctx)
    : m_Context(ctx) { }

RenderView EditorViewportController::GetActiveRenderView() {
	Camera* cam { nullptr };
	TransformComponent* transf { nullptr };

	if (m_Context.ViewMode == EditorViewMode::Game) {
		m_Context.ActiveScene->View<CameraComponent, TransformComponent>([&](RID e, CameraComponent& cc, TransformComponent& tr) {
			if (cam != nullptr)
				return;

			if (cc.IsPrimary) {
				cam = &cc.GetCamera();
				cam->UpdateFromTransform(tr);
				transf = &tr;
			}
		});
	}

	if (m_Context.ViewMode == EditorViewMode::Editor || !cam) {
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
