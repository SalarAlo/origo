#pragma once

#include "origo/scene/ISystem.h"
#include "origo/scene/Scene.h"

namespace Origo {
template <ComponentType T>
class SingleComponentSystem : public ISystem {
public:
	virtual void ForEach(const Scene& scene, const std::vector<T*>&) = 0;

	void Run(Scene& scene) override {
		auto instances = scene.GetAllComponentsOfType<T>();
		ForEach(scene, instances);
	}

private:
};

}
