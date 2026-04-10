#pragma once

#include "origo/scene/IUpdateSystem.h"

namespace Origo {
class CameraShakeComponentCounterSystem : public IUpdateSystem {
public:
	void update(Scene* scene, float dt) override;
};

}
