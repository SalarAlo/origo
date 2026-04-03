#pragma once

#include "origo/scene/IUpdateSystem.h"

namespace Origo {
class CameraShakeCounterSystem : public IUpdateSystem {
public:
	void update(Scene* scene, float dt) override;
};

}
