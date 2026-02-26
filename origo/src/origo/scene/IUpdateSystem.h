#pragma once
#include "origo/scene/Scene.h"

namespace Origo {

class IUpdateSystem {
public:
	virtual ~IUpdateSystem() = default;
	virtual void update(Scene* scene, float dt) = 0;
};

}
