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
	EditorContext(Origo::Scene* editorScene, Origo::FrameBuffer& renderBuffer, Origo::FrameBuffer& resolveBuffer, GLFWwindow* window, const EditorPalette& palette, Origo::LayerSystem& layerSystem)
	    : EditorScene(editorScene)
	    , ActiveScene(editorScene)
	    , RenderBuffer(renderBuffer)
	    , ResolveBuffer(resolveBuffer)
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

	Origo::LayerSystem& LayerSystem;

	GLFWwindow* Window;

	EditorPalette ColorPalette {};
	EditorCamera EditorViewportCamera {};

	EditorRuntimeState RuntimeState { EditorRuntimeState::Editing };
	EditorViewMode ViewMode { EditorViewMode::Editor };
	EditorViewportController ViewportController;

	void set_selected_entity(const Origo::RID& entity);
	void unselect_entity();
	std::optional<Origo::RID> get_selected_entity() const { return m_selected_entity; }

	void set_selected_asset(const Origo::UUID& asset);
	void unselect_asset();
	std::optional<Origo::UUID> get_selected_asset() const { return m_selected_asset; }

	EditorContext(const EditorContext&) = delete;
	void operator=(const EditorContext&) = delete;
	EditorContext(EditorContext&&) = delete;
	void operator=(EditorContext&&) = delete;

private:
	std::optional<Origo::RID> m_selected_entity { std::nullopt };
	OptionalUUID m_selected_asset { std::nullopt };
};

}
