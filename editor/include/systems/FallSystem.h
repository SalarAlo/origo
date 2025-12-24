#pragma once

#include "origo/scene/IUpdateSystem.h"
#include "origo/scene/Scene.h"

namespace OrigoEditor {

class FallSystem final : public Origo::IUpdateSystem {
public:
	void Update(Origo::Scene* scene, float dt) override;
};

}
