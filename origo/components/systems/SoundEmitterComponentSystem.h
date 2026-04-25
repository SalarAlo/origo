#pragma once

#include "origo/scene/IUpdateSystem.h"
#include "origo/scene/Scene.h"

namespace Origo {

class SoundEmitterComponentSystem final : public IUpdateSystem {
public:
	void update(Scene* scene, float dt) override;
};

}
