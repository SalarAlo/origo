#pragma once

#include "origo/scene/ISystem.h"
#include "origo/scene/Scene.h"

namespace Origo {
template <ComponentConcept T>
class SingleComponentSystem : public ISystem {
public:
	virtual void ForEach(const Scene& scene, const std::vector<Ref<T>>&) = 0;

	void Run(Scene& scene) override {
		auto instances = scene.GetAllComponentsOfType<T>();
		ForEach(scene, instances);
	}

private:
};

}
