#include "layers/RenderLayer.h"

#include "origo/assets/SkyboxDefaults.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

using namespace Origo;

namespace OrigoEditor {

namespace {
void render_view(EditorContext& context, RenderContext& renderContext, Origo::Scene* scene, EditorViewMode mode, double dt, bool runUpdateAlways) {
	if (!scene)
		return;

	const bool editing_view = mode == EditorViewMode::Editor;

	renderContext.set_target(&context.get_render_buffer(mode));
	renderContext.set_resolve_target(&context.get_resolve_buffer(mode));
	renderContext.set_view(context.ViewportController.get_render_view(mode, scene));
	renderContext.set_editor_grid_enabled(editing_view);

	renderContext.begin_frame();

	SystemScheduler::get_instance().run_phase(GamePhase::RenderAlways, scene, renderContext);
	if (runUpdateAlways)
		SystemScheduler::get_instance().run_phase(GamePhase::UpdateAlways, scene, dt);

	if (editing_view) {
		SystemScheduler::get_instance().run_phase(GamePhase::RenderEditor, scene, renderContext);
	} else if (context.RuntimeState == EditorRuntimeState::Running && scene == context.get_runtime_scene()) {
		SystemScheduler::get_instance().run_phase(GamePhase::RenderGame, scene, renderContext);
	}

	renderContext.flush();
	renderContext.end_frame();
}
}

void RenderLayer::on_attach() {
	m_render_context.set_skybox_material(SkyboxDefaults::get_instance().get_material());
}

void RenderLayer::on_update(double dt) {
	auto* viewport_scene = m_context.get_viewport_scene();

	if (m_context.is_viewport_visible(EditorViewMode::Editor)) {
		render_view(m_context, m_render_context, viewport_scene, EditorViewMode::Editor, dt, true);
	}

	if (m_context.is_viewport_visible(EditorViewMode::Game)) {
		render_view(
		    m_context,
		    m_render_context,
		    viewport_scene,
		    EditorViewMode::Game,
		    dt,
		    false);
	}
}

}
