#include "layers/UpdateLayer.h"

#include "origo/scene/SystemScheduler.h"

namespace OrigoEditor {

void UpdateLayer::on_update(double dt) {
	auto scene_ptr = m_context.get_runtime_scene();
	if (!scene_ptr)
		return;

	Origo::SystemScheduler::get_instance().run_phase(Origo::GamePhase::UpdateGame, scene_ptr, dt);
}

}
