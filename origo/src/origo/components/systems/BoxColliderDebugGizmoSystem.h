#pragma once

#include "origo/scene/IRenderSystem.h"

namespace Origo {
class BoxColliderDebugGizmoSystem : public IRenderSystem {
public:
	void render(Scene*, RenderContext&) override;
};

}
