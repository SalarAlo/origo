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

	Origo::Scene* EditorScene;
	Origo::Scene* RuntimeScene { nullptr };

	Origo::Scene* ActiveScene;

	Origo::FrameBuffer& RenderBuffer;
	Origo::FrameBuffer& ResolveBuffer;

	Origo::LayerSystem& LayerSystem;

	GLFWwindow* Window;

	EditorPalette ColorPalette {};
	EditorCamera EditorViewportCamera {};

	EditorRuntimeState RuntimeState { EditorRuntimeState::Editing };
	EditorViewMode ViewMode { EditorViewMode::Editor };
	EditorViewportController ViewportController;

	void SetSelectedEntity(const Origo::RID& entity);
	void UnselectSelectedEntity();
	std::optional<Origo::RID> GetSelectedEntity() const { return m_SelectedEntity; }

	EditorContext(const EditorContext&) = delete;
	void operator=(const EditorContext&) = delete;
	EditorContext(EditorContext&&) = delete;
	void operator=(EditorContext&&) = delete;

private:
	std::optional<Origo::RID> m_SelectedEntity { std::nullopt };
};

}
