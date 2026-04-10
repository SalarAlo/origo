#pragma once

#include "origo/scene/IRenderSystem.h"
#include "origo/scene/Scene.h"

namespace Origo {

class DirectionalLightComponentSystem final : public Origo::IRenderSystem {
public:
	void render(Scene*, RenderContext&) override;
};

}
