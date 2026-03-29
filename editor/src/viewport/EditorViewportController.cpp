#include "viewport/EditorViewportController.h"

#include "origo/components/CameraComponent.h"
#include "origo/components/CameraShakeComponent.h"
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
	CameraShakeComponent* shake { nullptr };

	RID camera_entity {};

	if (mode == EditorViewMode::Game && scene) {
		scene->view<CameraComponent, TransformComponent, CameraShakeComponent>(
		    [&](RID e, CameraComponent& cc, TransformComponent& tr, CameraShakeComponent& sh) {
			    if (cam != nullptr)
				    return;

			    if (cc.IsPrimary) {
				    cam = &cc.CameraObj;
				    transf = &tr;
				    shake = &sh;
				    camera_entity = e;
			    }
		    });

		if (!cam) {
			scene->view<CameraComponent, TransformComponent>(
			    [&](RID e, CameraComponent& cc, TransformComponent& tr) {
				    if (cam != nullptr)
					    return;

				    if (cc.IsPrimary) {
					    cam = &cc.CameraObj;
					    transf = &tr;
					    camera_entity = e;
				    }
			    });
		}
	}

	if (mode == EditorViewMode::Editor || !cam) {
		cam = &m_context.EditorViewportCamera.get_camera();
		transf = &m_context.EditorViewportCamera.get_transform();
		shake = nullptr;
	}

	TransformComponent final = *transf;

	if (shake) {
		float total = shake->ShakeDelay + shake->ShakeDuration;

		if (shake->ShakeCounter >= shake->ShakeDelay && shake->ShakeCounter < total) {

			float t = shake->ShakeCounter - shake->ShakeDelay;

			float decay = 1.0f - (t / shake->ShakeDuration);
			decay = glm::clamp(decay, 0.0f, 1.0f);

			float freq = shake->ShakeFrequency;
			float amp = shake->ShakeAmplitude * decay;

			float x = sin(t * freq * 1.3f) * amp;
			float y = cos(t * freq * 1.7f) * amp;

			final.set_position(final.get_position() + glm::vec3(x, y, 0.0f));
		}
	}

	// --- UPDATE CAMERA WITH FINAL TRANSFORM ---
	cam->update_from_transform(final);

	// --- NORMAL RENDER VIEW SETUP ---
	const auto& render_buffer = m_context.get_render_buffer(mode);
	float width = static_cast<float>(render_buffer.get_width());
	float height = static_cast<float>(render_buffer.get_height());

	cam->set_aspect(width / height);

	RenderView view {
		.Projection = cam->get_projection(),
		.View = cam->get_view(),
		.CameraForward = final.get_forward(),
		.CameraPosition = final.get_position(),
	};

	return view;
}

}
