#pragma once
#include "origo/scene/Scene.h"

namespace Origo {
class ISystem {
public:
	virtual ~ISystem() = default;
	virtual void Run(Scene& scene) = 0;
};
}
