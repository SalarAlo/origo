#pragma once

#include "origo/scene/IUpdateSystem.h"
#include "origo/scene/Scene.h"

namespace Origo {

class ParticleSimulationSystem final : public IUpdateSystem {
public:
	void update(Scene* scene, float dt) override;
};

}
