#pragma once

#include "origo/core/LayerSystem.h"

#include "origo/renderer/FrameBuffer.h"

#include "origo/scene/Scene.h"

#include "state/EditorRuntimeState.h"
#include "state/EditorViewMode.h"

#include "systems/EditorCamera.h"

#include "ui/EditorPalette.h"

#include "viewport/EditorViewportController.h"

namespace OrigoEditor {

struct EditorContext {
	EditorContext(Origo::Scene* editorScene, Origo::FrameBuffer& renderBuffer, Origo::FrameBuffer& resolveBuffer, Origo::FrameBuffer& editorPickBuffer, Origo::FrameBuffer& gameRenderBuffer, Origo::FrameBuffer& gameResolveBuffer, GLFWwindow* window, const EditorPalette& palette, Origo::LayerSystem& layerSystem)
	    : EditorScene(editorScene)
	    , ActiveScene(editorScene)
	    , RenderBuffer(renderBuffer)
	    , ResolveBuffer(resolveBuffer)
	    , EditorPickBuffer(editorPickBuffer)
	    , GameRenderBuffer(gameRenderBuffer)
	    , GameResolveBuffer(gameResolveBuffer)
	    , Window(window)
	    , LayerSystem(layerSystem)
	    , ViewportController(*this)
	    , ColorPalette(palette) { }

	Origo::Scope<Origo::Scene> EditorScene;
	Origo::Scope<Origo::Scene> RuntimeScene { nullptr };
	Origo::Scope<Origo::Scene> PendingScene { nullptr };

	Origo::Scene* ActiveScene;

	float DeltaTime;

	Origo::FrameBuffer& RenderBuffer;
	Origo::FrameBuffer& ResolveBuffer;
	Origo::FrameBuffer& EditorPickBuffer;
	Origo::FrameBuffer& GameRenderBuffer;
	Origo::FrameBuffer& GameResolveBuffer;

	Origo::LayerSystem& LayerSystem;

	GLFWwindow* Window;

	EditorPalette ColorPalette {};
	EditorCamera EditorViewportCamera {};

	EditorRuntimeState RuntimeState { EditorRuntimeState::Editing };
	EditorViewportController ViewportController;

	Origo::Scene* get_editor_scene() const { return EditorScene.get(); }
	Origo::Scene* get_runtime_scene() const { return RuntimeState == EditorRuntimeState::Running ? RuntimeScene.get() : nullptr; }
	Origo::Scene* get_viewport_scene() const { return ActiveScene; }
	Origo::Scene* get_game_scene() const { return ActiveScene; }

	Origo::FrameBuffer& get_render_buffer(EditorViewMode mode) {
		return mode == EditorViewMode::Editor ? RenderBuffer : GameRenderBuffer;
	}

	const Origo::FrameBuffer& get_render_buffer(EditorViewMode mode) const {
		return mode == EditorViewMode::Editor ? RenderBuffer : GameRenderBuffer;
	}

	Origo::FrameBuffer& get_resolve_buffer(EditorViewMode mode) {
		return mode == EditorViewMode::Editor ? ResolveBuffer : GameResolveBuffer;
	}

	const Origo::FrameBuffer& get_resolve_buffer(EditorViewMode mode) const {
		return mode == EditorViewMode::Editor ? ResolveBuffer : GameResolveBuffer;
	}

	Origo::FrameBuffer& get_pick_buffer(EditorViewMode mode) {
		return mode == EditorViewMode::Editor ? EditorPickBuffer : GameResolveBuffer;
	}

	const Origo::FrameBuffer& get_pick_buffer(EditorViewMode mode) const {
		return mode == EditorViewMode::Editor ? EditorPickBuffer : GameResolveBuffer;
	}

	void set_viewport_visible(EditorViewMode mode, bool visible) {
		if (mode == EditorViewMode::Editor)
			m_editor_viewport_visible = visible;
		else
			m_game_viewport_visible = visible;
	}

	bool is_viewport_visible(EditorViewMode mode) const {
		return mode == EditorViewMode::Editor ? m_editor_viewport_visible : m_game_viewport_visible;
	}

	void set_selected_entity(const Origo::RID& entity);
	void unselect_entity();
	std::optional<Origo::RID> get_selected_entity() const { return m_selected_entity; }

	void set_selected_asset(const Origo::UUID& asset);
	void unselect_asset();
	std::optional<Origo::UUID> get_selected_asset() const { return m_selected_asset; }
	void mark_text_input_active() { m_text_input_active = true; }
	void clear_text_input_active() { m_text_input_active = false; }
	bool is_text_input_active() const { return m_text_input_active; }
	void set_skybox_enabled(bool enabled) { m_skybox_enabled = enabled; }
	bool is_skybox_enabled() const { return m_skybox_enabled; }

	EditorContext(const EditorContext&) = delete;
	void operator=(const EditorContext&) = delete;
	EditorContext(EditorContext&&) = delete;
	void operator=(EditorContext&&) = delete;

private:
	std::optional<Origo::RID> m_selected_entity { std::nullopt };
	OptionalUUID m_selected_asset { std::nullopt };
	bool m_text_input_active { false };
	bool m_editor_viewport_visible { false };
	bool m_game_viewport_visible { false };
	bool m_skybox_enabled { true };
};

}
