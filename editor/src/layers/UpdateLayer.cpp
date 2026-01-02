#include "layer/UpdateLayer.h"
#include "origo/scene/SystemScheduler.h"

namespace OrigoEditor {

void UpdateLayer::OnUpdate(double dt) {
	auto scenePtr = m_Context.ActiveScene;
	Origo::SystemScheduler::Get().RunPhase(Origo::GamePhase::Update, scenePtr, dt);
}

}
