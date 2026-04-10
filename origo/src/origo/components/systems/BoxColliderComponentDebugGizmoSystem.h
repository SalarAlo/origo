#pragma once

#include "origo/scene/IRenderSystem.h"

namespace Origo {
class BoxColliderComponentDebugGizmoSystem : public IRenderSystem {
public:
	void render(Scene*, RenderContext&) override;
};

}
