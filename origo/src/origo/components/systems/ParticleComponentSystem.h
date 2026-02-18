#pragma once

#include "origo/scene/IUpdateSystem.h"
#include "origo/scene/Scene.h"

namespace Origo {

class ParticleComponentSystem final : public IUpdateSystem {
public:
	void Update(Scene* scene, float dt) override;
};

}
