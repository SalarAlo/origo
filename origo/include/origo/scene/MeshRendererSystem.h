#pragma once

#include "origo/scene/SingleComponentSystem.h"
#include "origo/scene/MeshRenderer.h"

namespace Origo {
class MeshRendererSystem : public SingleComponentSystem<MeshRenderer> {
public:
	void ForEach(const Scene& scene, const std::vector<MeshRenderer*>& instances) override;
};
}
