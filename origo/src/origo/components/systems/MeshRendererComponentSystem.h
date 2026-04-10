#pragma once

#include "origo/scene/IRenderSystem.h"
#include "origo/scene/Scene.h"
#include "origo/renderer/RenderContext.h"

namespace Origo {

class MeshRendererComponentSystem final : public IRenderSystem {
public:
	void render(Scene* scene, RenderContext& context) override;
};

}
