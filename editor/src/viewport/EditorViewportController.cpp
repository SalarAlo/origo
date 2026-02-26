#include "viewport/EditorViewportController.h"

#include "origo/components/CameraComponent.h"
#include "origo/components/Transform.h"

#include "origo/renderer/RenderView.h"

#include "state/EditorContext.h"
#include "state/EditorViewMode.h"

using namespace Origo;
namespace OrigoEditor {

EditorViewportController::EditorViewportController(EditorContext& ctx)
    : m_context(ctx) { }

RenderView EditorViewportController::get_and_update_active_render_view() {
	Camera* cam { nullptr };
	TransformComponent* transf { nullptr };

	if (m_context.ViewMode == EditorViewMode::Game) {
		m_context.ActiveScene->view<CameraComponent, TransformComponent>([&](RID e, CameraComponent& cc, TransformComponent& tr) {
			if (cam != nullptr)
				return;

			if (cc.IsPrimary) {
				cam = &cc.CameraObj;
				cam->update_from_transform(tr);
				transf = &tr;
			}
		});
	}

	if (m_context.ViewMode == EditorViewMode::Editor || !cam) {
		cam = &m_context.EditorViewportCamera.get_camera();
		transf = &m_context.EditorViewportCamera.get_transform();
	}

	float width { static_cast<float>(m_context.RenderBuffer.get_width()) };
	float height { static_cast<float>(m_context.RenderBuffer.get_height()) };
	cam->set_aspect(width / height);

	RenderView view {
		.Projection = cam->get_projection(),
		.View = cam->get_view(),
		.CameraForward = transf->get_forward(),
		.CameraPosition = transf->get_position(),
	};

	return view;
}

}
