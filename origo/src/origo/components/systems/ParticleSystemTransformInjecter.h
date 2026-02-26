
#pragma once

#include "origo/scene/IUpdateSystem.h"
#include "origo/scene/Scene.h"

namespace Origo {

class ParticleSystemTransformInjecter final : public Origo::IUpdateSystem {
public:
	void update(Origo::Scene* scene, float dt) override;
};
}
