#include "systems/EditorRuntimeController.h"

namespace OrigoEditor {

void EditorRuntimeController::Play() {
	if (m_Context.RuntimeState != EditorRuntimeState::Editing)
		return;

	m_Context.RuntimeScene = new Origo::Scene(*m_Context.EditorScene);
	m_Context.ActiveScene = m_Context.RuntimeScene;
	m_Context.RuntimeState = EditorRuntimeState::Running;

	ORG_INFO("Play mode started");
}

void EditorRuntimeController::Stop() {
	if (m_Context.RuntimeState != EditorRuntimeState::Running)
		return;

	delete m_Context.RuntimeScene;

	m_Context.RuntimeScene = nullptr;
	m_Context.ActiveScene = m_Context.EditorScene;
	m_Context.RuntimeState = EditorRuntimeState::Editing;
}

}
