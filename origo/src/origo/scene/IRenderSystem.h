#pragma once

#include "origo/renderer/RenderContext.h"
#include "origo/scene/Scene.h"

namespace Origo {

struct IRenderSystem {
	virtual ~IRenderSystem() = default;
	virtual void Render(Scene*, RenderContext&) = 0;
};

};
