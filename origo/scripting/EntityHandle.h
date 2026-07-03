#pragma once

#include "origo/core/RID.h"

namespace Origo {

class Scene;

struct EntityHandle {
	Scene* OwnerScene {};
	RID Entity {};
	uint32_t Generation {};

	bool is_valid() const;
	void translate(float x, float y, float z) const;
};

}
