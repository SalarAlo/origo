#pragma once

#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Entity.hpp"
#include "origo/scene/Scene.h"

namespace OrigoEditor {

struct EditorContext {
	Origo::Scene& Scene;
	Origo::FrameBuffer& Buffer;
	GLFWwindow* Window;
	Origo::Entity* SelectedEntity;
};

}
