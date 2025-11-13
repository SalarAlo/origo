#pragma once

#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/Scene.h"

namespace OrigoEditor {

struct EditorContext {
	EditorContext(Origo::Scene& scene, Origo::FrameBuffer& buffer, GLFWwindow* window)
	    : Scene(scene)
	    , Buffer(buffer)
	    , Window(window) { }

	Origo::Scene& Scene;
	Origo::FrameBuffer& Buffer;
	GLFWwindow* Window;
	Origo::Entity* SelectedEntity { nullptr };

	EditorContext(const EditorContext&) = delete;
	EditorContext& operator=(const EditorContext&) = delete;

	EditorContext(EditorContext&&) = delete;
	EditorContext& operator=(EditorContext&&) = delete;
};

}
