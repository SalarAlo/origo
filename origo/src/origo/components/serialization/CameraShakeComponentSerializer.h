#pragma once

#include "origo/components/serialization/IComponentSerializer.h"

namespace Origo {
class CameraShakeComponentSerializer : public IComponentSerializer {
public:
	void serialize(Component*, ISerializer&) const override;
	void deserialize(Component*, ISerializer&) override;
};

}
