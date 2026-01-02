#include "systems/EditorRuntimeController.h"
#include "state/EditorViewMode.h"

namespace OrigoEditor {

void EditorRuntimeController::Play() {
	if (m_Context.RuntimeState != EditorRuntimeState::Editing)
		return;

	m_Context.RuntimeScene = new Origo::Scene(*m_Context.EditorScene);
	m_Context.ActiveScene = m_Context.RuntimeScene;
	m_Context.RuntimeState = EditorRuntimeState::Running;
	m_Context.ViewMode = EditorViewMode::Run;

	m_Context.UnselectSelectedEntity();

	ORG_INFO("Play mode started");
}

void EditorRuntimeController::Stop() {

	if (m_Context.RuntimeState != EditorRuntimeState::Running)
		return;

	delete m_Context.RuntimeScene;

	m_Context.RuntimeScene = nullptr;
	m_Context.ActiveScene = m_Context.EditorScene;
	m_Context.RuntimeState = EditorRuntimeState::Editing;
	m_Context.ViewMode = EditorViewMode::Editor;

	m_Context.UnselectSelectedEntity();
}

}
