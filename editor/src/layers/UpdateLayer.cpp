#include "layers/UpdateLayer.h"

#include "origo/scene/SystemScheduler.h"

namespace OrigoEditor {

void UpdateLayer::on_update(double dt) {
	auto scene_ptr = m_context.ActiveScene;
	Origo::SystemScheduler::get_instance().run_phase(Origo::GamePhase::UpdateGame, scene_ptr, dt);
	Origo::SystemScheduler::get_instance().run_phase(Origo::GamePhase::RenderGame, scene_ptr, m_render_context);
}

}
