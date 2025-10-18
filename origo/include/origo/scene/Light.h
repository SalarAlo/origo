#pragma once

#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
enum LightType {
	Directional,
	Point,
};

class Light : public Component {
public:
	Light(LightType type, Entity& entity);

private:
	LightType m_Type;
};
}
