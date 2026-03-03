#include "systems/EditorRuntimeController.h"

#include "layers/LayerType.h"

#include "origo/core/Logger.h"

#include "state/EditorViewMode.h"
#include "ui/EditorNotificationSystem.h"

namespace OrigoEditor {

static constexpr size_t update_layer_key { static_cast<size_t>(LayerType::UpdateLayer) };
namespace {
	constexpr float confirmation_duration_seconds = 2.2f;
	constexpr float important_duration_seconds = 4.5f;
}

bool EditorRuntimeController::can_step() const { return can_resume(); }
void EditorRuntimeController::step() {
	if (!can_step())
		return;
	m_context.LayerSystem.request_activate_layer(update_layer_key, [&]() { ORG_CORE_TRACE("Step made"); });
	EditorNotificationSystem::get_instance().info(
	    "Step",
	    "Advanced the runtime by one frame.",
	    confirmation_duration_seconds);
}

bool EditorRuntimeController::can_play() const { return m_context.RuntimeState == EditorRuntimeState::Editing; }
void EditorRuntimeController::play() {
	if (!can_play())
		return;

	m_context.RuntimeScene = MakeScope<Origo::Scene>(*m_context.EditorScene);
	m_context.ActiveScene = m_context.RuntimeScene.get();
	m_context.RuntimeState = EditorRuntimeState::Running;
	m_context.ViewMode = EditorViewMode::Game;

	m_context.unselect_entity();

	m_context.LayerSystem.request_activate_layer(update_layer_key);

	ORG_CORE_TRACE("Play mode started");
	EditorNotificationSystem::get_instance().info(
	    "Game Started",
	    "Entered play mode and switched to the game view.",
	    important_duration_seconds);
}

bool EditorRuntimeController::can_pause() const { return m_context.RuntimeState == EditorRuntimeState::Running && m_context.LayerSystem.has_active_layer(update_layer_key); }
void EditorRuntimeController::pause(bool changeToEditorView) {
	if (!can_pause())
		return;
	m_context.LayerSystem.request_freeze_layer(update_layer_key);
	if (changeToEditorView)
		m_context.ViewMode = EditorViewMode::Editor;

	ORG_CORE_TRACE("Pause");
	EditorNotificationSystem::get_instance().info(
	    "Game Paused",
	    changeToEditorView ? "Runtime paused and returned to the editor view." : "Runtime paused.",
	    confirmation_duration_seconds);
}

bool EditorRuntimeController::can_resume() const { return m_context.RuntimeState == EditorRuntimeState::Running && !m_context.LayerSystem.has_active_layer(update_layer_key); }
void EditorRuntimeController::resume() {
	if (!can_resume())
		return;

	m_context.LayerSystem.request_activate_layer(update_layer_key);
	m_context.ViewMode = EditorViewMode::Game;

	ORG_CORE_TRACE("Resume");
	EditorNotificationSystem::get_instance().info(
	    "Game Resumed",
	    "Runtime updates resumed.",
	    confirmation_duration_seconds);
}

bool EditorRuntimeController::can_stop() const { return m_context.RuntimeState == EditorRuntimeState::Running; }
void EditorRuntimeController::stop() {
	if (!can_stop())
		return;

	m_context.RuntimeScene.reset();
	m_context.ActiveScene = m_context.EditorScene.get();

	m_context.RuntimeState = EditorRuntimeState::Editing;
	m_context.ViewMode = EditorViewMode::Editor;

	if (m_context.LayerSystem.has_active_layer(update_layer_key)) {
		m_context.LayerSystem.request_freeze_layer(update_layer_key);
	}
	m_context.unselect_entity();
	EditorNotificationSystem::get_instance().success(
	    "Game Stopped",
	    "Returned to editing mode.",
	    confirmation_duration_seconds);
}

}
