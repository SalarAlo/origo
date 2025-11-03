#pragma once

#include "origo/renderer/FrameBuffer.h"
#include "origo/scene/Scene.h"
#include "panels/PanelManager.h"

namespace OrigoEditor {

struct EditorContext {
	Origo::Scene& Scene;
	Origo::Ref<Origo::Camera> Camera;
	Origo::FrameBuffer& Buffer;
	PanelManager& Panels;
};

}
