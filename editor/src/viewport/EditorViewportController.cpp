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

RenderView EditorViewportController::get_render_view(EditorViewMode mode, Origo::Scene* scene) {
	Camera* cam { nullptr };
	TransformComponent* transf { nullptr };

	if (mode == EditorViewMode::Game && scene) {
		scene->view<CameraComponent, TransformComponent>([&](RID e, CameraComponent& cc, TransformComponent& tr) {
			if (cam != nullptr)
				return;

			if (cc.IsPrimary) {
				cam = &cc.CameraObj;
				cam->update_from_transform(tr);
				transf = &tr;
			}
		});
	}

	if (mode == EditorViewMode::Editor || !cam) {
		cam = &m_context.EditorViewportCamera.get_camera();
		transf = &m_context.EditorViewportCamera.get_transform();
	}

	const auto& render_buffer = m_context.get_render_buffer(mode);
	float width { static_cast<float>(render_buffer.get_width()) };
	float height { static_cast<float>(render_buffer.get_height()) };
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
