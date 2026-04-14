#include "layers/RenderLayer.h"

#include "origo/assets/SkyboxDefaults.h"

#include "origo/components/CameraComponent.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

using namespace Origo;

namespace OrigoEditor {

namespace {
	PostProcessSettings default_post_process_settings() {
		return PostProcessSettings {};
	}

	void render_view(EditorContext& context, RenderContext& renderContext, Origo::Scene* scene, EditorViewMode mode, double dt) {
		if (!scene)
			return;

		const bool editing_view = mode == EditorViewMode::Editor;
		const auto* active_camera = context.ViewportController.get_active_camera_component(mode, scene);
		const PostProcessSettings post_settings = active_camera ? active_camera->PostProcess : default_post_process_settings();

		renderContext.set_target(&context.get_render_buffer(mode));
		renderContext.set_resolve_target(&context.get_resolve_buffer(mode));
		renderContext.set_present_target(&context.get_present_buffer(mode));
		renderContext.set_post_process_settings(post_settings);
		renderContext.set_view(context.ViewportController.get_render_view(mode, scene));
		renderContext.set_editor_grid_enabled(editing_view);

		renderContext.begin_frame();

		SystemScheduler::get_instance().run_phase(GamePhase::RenderAlways, scene, renderContext);
		SystemScheduler::get_instance().run_phase(GamePhase::UpdateAlways, scene, dt);

		if (editing_view) {
			SystemScheduler::get_instance().run_phase(GamePhase::RenderEditor, scene, renderContext);
		} else if (context.RuntimeState == EditorRuntimeState::Running && scene == context.get_runtime_scene()) {
			SystemScheduler::get_instance().run_phase(GamePhase::RenderGame, scene, renderContext);
		}

		renderContext.flush();
		renderContext.end_frame();
	}

	void render_pick_view(EditorContext& context, RenderContext& renderContext, Origo::Scene* scene, double dt) {
		if (!scene)
			return;

		renderContext.set_target(&context.get_pick_buffer(EditorViewMode::Editor));
		renderContext.set_resolve_target(nullptr);
		renderContext.set_present_target(nullptr);
		renderContext.clear_post_process_settings();
		renderContext.set_view(context.ViewportController.get_render_view(EditorViewMode::Editor, scene));
		renderContext.set_editor_grid_enabled(false);

		renderContext.begin_frame();
		SystemScheduler::get_instance().run_phase(GamePhase::RenderAlways, scene, renderContext);
		SystemScheduler::get_instance().run_phase(GamePhase::UpdateAlways, scene, dt);
		renderContext.flush();
		renderContext.end_frame();
	}
}

void RenderLayer::on_attach() {
	m_render_context.set_skybox_material(SkyboxDefaults::get_instance().get_material());
	m_pick_render_context.set_skybox_material(SkyboxDefaults::get_instance().get_material());
}

void RenderLayer::on_update(double dt) {
	if (m_context.is_skybox_enabled()) {
		const auto skybox = SkyboxDefaults::get_instance().get_material();
		m_render_context.set_skybox_material(skybox);
		m_pick_render_context.set_skybox_material(skybox);
	} else {
		m_render_context.clear_skybox_material();
		m_pick_render_context.clear_skybox_material();
	}

	auto* viewport_scene = m_context.get_viewport_scene();

	if (m_context.is_viewport_visible(EditorViewMode::Editor)) {
		render_view(m_context, m_render_context, viewport_scene, EditorViewMode::Editor, dt);
		render_pick_view(m_context, m_pick_render_context, viewport_scene, dt);
	}

	if (m_context.is_viewport_visible(EditorViewMode::Game)) {
		render_view(
		    m_context,
		    m_render_context,
		    viewport_scene,
		    EditorViewMode::Game,
		    dt);
	}
}

}
