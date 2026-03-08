#include "layers/RenderLayer.h"

#include "origo/assets/SkyboxDefaults.h"

#include "origo/scene/GamePhase.h"
#include "origo/scene/SystemScheduler.h"

using namespace Origo;

namespace OrigoEditor {

void RenderLayer::on_attach() {
	m_render_context.set_skybox_material(SkyboxDefaults::get_instance().get_material());
}

void RenderLayer::on_update(double dt) {
	const bool editing_view = (m_context.ViewMode == EditorViewMode::Editor);

	m_render_context.set_view(m_context.ViewportController.get_and_update_active_render_view());
	m_render_context.set_editor_grid_enabled(editing_view);

	m_render_context.begin_frame();

	auto active_scene_ptr = m_context.ActiveScene;

	SystemScheduler::get_instance().run_phase(GamePhase::RenderAlways, active_scene_ptr, m_render_context);
	SystemScheduler::get_instance().run_phase(GamePhase::UpdateAlways, active_scene_ptr, dt);

	if (editing_view) {
		SystemScheduler::get_instance().run_phase(GamePhase::RenderEditor, active_scene_ptr, m_render_context);
	}

	m_render_context.flush();

	m_render_context.end_frame();
}

}
