#pragma once

#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Scene.h"

namespace OrigoEditor {

struct EditorContext {
	Origo::Scene& Scene;
	Origo::FrameBuffer& Buffer;
	GLFWwindow* Window;
};

}
