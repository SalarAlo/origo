#pragma once

#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/Scene.h"
#include "ui/EditorPalette.h"

namespace OrigoEditor {

struct EditorContext {
	EditorContext(Origo::Scene& scene, Origo::FrameBuffer& renderBuffer, Origo::FrameBuffer& resolveBuffer, GLFWwindow* window, EditorPalette palette)
	    : Scene(scene)
	    , RenderBuffer(renderBuffer)
	    , ResolveBuffer(resolveBuffer)
	    , Window(window)
	    , ColorPalette(palette) { }

	Origo::Scene& Scene;
	Origo::FrameBuffer& RenderBuffer;
	Origo::FrameBuffer& ResolveBuffer;
	GLFWwindow* Window;
	EditorPalette ColorPalette;
	std::optional<Origo::Entity> SelectedEntity { std::nullopt };

	EditorContext(const EditorContext&) = delete;
	EditorContext& operator=(const EditorContext&) = delete;

	EditorContext(EditorContext&&) = delete;
	EditorContext& operator=(EditorContext&&) = delete;
};

}
