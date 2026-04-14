#include "viewport/EditorViewportController.h"

#include "origo/components/CameraComponent.h"
#include "origo/components/CameraShakeComponent.h"
#include "origo/components/TransformComponent.h"

#include "origo/renderer/RenderView.h"

#include "state/EditorContext.h"
#include "state/EditorViewMode.h"

using namespace Origo;
namespace OrigoEditor {

EditorViewportController::EditorViewportController(EditorContext& ctx)
    : m_context(ctx) { }

RenderView EditorViewportController::get_render_view(EditorViewMode mode, Origo::Scene* scene) {
	Camera* cam { nullptr };
	std::optional<TransformComponent> transform {};
	CameraShakeComponent* shake { nullptr };

	RID camera_entity {};

	if (mode == EditorViewMode::Game && scene) {
		scene->view<CameraComponent, TransformComponent>(
		    [&](RID e, CameraComponent& cc, TransformComponent& tr) {
			    if (cam != nullptr)
				    return;

			    if (cc.IsPrimary) {
				    cam = &cc.CameraObj;
				    transform = tr;
				    camera_entity = e;
				    shake = scene->get_native_component<CameraShakeComponent>(camera_entity);
			    }
		    });
	}

	if (shake && shake->Counter >= shake->Delay && m_context.RuntimeState == EditorRuntimeState::Running && shake->Counter < shake->Delay + shake->Duration) {
		float time_passed { shake->Counter - shake->Delay };
		float normalized { time_passed / shake->Duration };
		float inverse_normalized { 1 - normalized };
		float amplitude { (shake->Amplitude * inverse_normalized) };

		float x { glm::sin(time_passed * shake->Frequency) * amplitude };
		float y { glm::cos(time_passed * shake->Frequency) * amplitude };

		float rot_x { glm::sin(time_passed * shake->Frequency + 4.2f) * amplitude };
		float rot_y { glm::cos(time_passed * shake->Frequency + 6.7f) * amplitude };

		transform->set_position(transform->get_position() + transform->get_right() * x + transform->get_up() * y);
		transform->set_rotation(transform->get_rotation() + Vec3 { rot_x, 0, rot_y });
	}

	if (mode == EditorViewMode::Editor || !cam) {
		cam = &m_context.EditorViewportCamera.get_camera();
		transform = m_context.EditorViewportCamera.get_transform();
	}

	cam->update_from_transform(*transform);

	const auto& render_buffer = m_context.get_render_buffer(mode);
	float width = static_cast<float>(render_buffer.get_width());
	float height = static_cast<float>(render_buffer.get_height());

	cam->set_aspect(width / height);

	RenderView view {
		.Projection = cam->get_projection(),
		.View = cam->get_view(),
		.CameraForward = transform->get_forward(),
		.CameraPosition = transform->get_position(),
	};

	return view;
}

const CameraComponent* EditorViewportController::get_active_camera_component(EditorViewMode mode, Origo::Scene* scene) const {
	if (mode != EditorViewMode::Game || !scene)
		return nullptr;

	const CameraComponent* active_camera = nullptr;

	scene->view<CameraComponent>([&](RID, CameraComponent& cc) {
		if (!active_camera && cc.IsPrimary)
			active_camera = &cc;
	});

	return active_camera;
}

}
