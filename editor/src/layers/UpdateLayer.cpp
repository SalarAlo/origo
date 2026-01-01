#include "layer/UpdateLayer.h"
#include "origo/scene/SystemScheduler.h"

namespace OrigoEditor {

void UpdateLayer::OnUpdate(double dt) {
	auto scenePtr = m_Context.ActiveScene;
	if (m_Context.RuntimeState == EditorRuntimeState::Running) {
		Origo::SystemScheduler::Get().RunPhase(Origo::GamePhase::Update, scenePtr, dt);
	}
}

}
