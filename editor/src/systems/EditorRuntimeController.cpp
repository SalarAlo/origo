#include "systems/EditorRuntimeController.h"
#include "layer/LayerType.h"
#include "origo/core/Logger.h"
#include "state/EditorViewMode.h"

namespace OrigoEditor {

static constexpr size_t UPDATE_LAYER_KEY { static_cast<size_t>(LayerType::UpdateLayer) };

bool EditorRuntimeController::CanPlay() const { return m_Context.RuntimeState == EditorRuntimeState::Editing; }
void EditorRuntimeController::Play() {
	if (!CanPlay())
		return;

	m_Context.RuntimeScene = new Origo::Scene(*m_Context.EditorScene);
	m_Context.ActiveScene = m_Context.RuntimeScene;
	m_Context.RuntimeState = EditorRuntimeState::Running;
	m_Context.ViewMode = EditorViewMode::Game;

	m_Context.UnselectSelectedEntity();

	m_Context.LayerSystem.RequestActivateLayer(UPDATE_LAYER_KEY);

	ORG_INFO("Play mode started");
}

bool EditorRuntimeController::CanPause() const { return m_Context.RuntimeState == EditorRuntimeState::Running && m_Context.LayerSystem.HasActiveLayer(UPDATE_LAYER_KEY); }
void EditorRuntimeController::Pause() {
	if (!CanPause()) {
		ORG_INFO("{}", !m_Context.LayerSystem.HasActiveLayer(UPDATE_LAYER_KEY));
		return;
	}

	m_Context.LayerSystem.RequestFreezeLayer(UPDATE_LAYER_KEY);
	m_Context.ViewMode = EditorViewMode::Editor;

	ORG_INFO("Pause started");
}

bool EditorRuntimeController::CanResume() const { return m_Context.RuntimeState == EditorRuntimeState::Running && !m_Context.LayerSystem.HasActiveLayer(UPDATE_LAYER_KEY); }

void EditorRuntimeController::Resume() {
	if (!CanResume()) {
		return;
	}

	m_Context.LayerSystem.RequestActivateLayer(UPDATE_LAYER_KEY);
	m_Context.ViewMode = EditorViewMode::Game;

	ORG_INFO("Pause started");
}

bool EditorRuntimeController::CanStop() const { return m_Context.RuntimeState == EditorRuntimeState::Running; }

void EditorRuntimeController::Stop() {
	if (!CanStop())
		return;

	delete m_Context.RuntimeScene;

	m_Context.RuntimeScene = nullptr;
	m_Context.ActiveScene = m_Context.EditorScene;
	m_Context.RuntimeState = EditorRuntimeState::Editing;
	m_Context.ViewMode = EditorViewMode::Editor;

	m_Context.LayerSystem.RequestFreezeLayer(UPDATE_LAYER_KEY);
	m_Context.UnselectSelectedEntity();
}

}
