#pragma once

#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Scene.h"
#include "state/EditorRuntimeState.h"
#include "state/EditorViewMode.h"
#include "systems/EditorCamera.h"
#include "ui/EditorPalette.h"
#include "viewport/EditorViewportController.h"

namespace OrigoEditor {

struct EditorContext {
	EditorContext(Origo::Scene* editorScene, Origo::FrameBuffer& renderBuffer, Origo::FrameBuffer& resolveBuffer, GLFWwindow* window, const EditorPalette& palette)
	    : EditorScene(editorScene)
	    , ActiveScene(editorScene)
	    , RenderBuffer(renderBuffer)
	    , ResolveBuffer(resolveBuffer)
	    , Window(window)
	    , ViewportController(*this)
	    , ColorPalette(palette) { }

	Origo::Scene* EditorScene;
	Origo::Scene* RuntimeScene { nullptr };

	Origo::Scene* ActiveScene;

	Origo::FrameBuffer& RenderBuffer;
	Origo::FrameBuffer& ResolveBuffer;

	GLFWwindow* Window;

	EditorPalette ColorPalette {};
	EditorCamera EditorViewportCamera {};

	EditorRuntimeState RuntimeState { EditorRuntimeState::Editing };
	EditorViewMode ViewMode { EditorViewMode::Editor };
	EditorViewportController ViewportController;

	std::optional<Origo::RID> SelectedEntity { std::nullopt };

	EditorContext(const EditorContext&) = delete;
	EditorContext& operator=(const EditorContext&) = delete;
	EditorContext(EditorContext&&) = delete;
	EditorContext& operator=(EditorContext&&) = delete;
};

}
