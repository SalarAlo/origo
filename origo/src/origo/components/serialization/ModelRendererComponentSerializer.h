#pragma once

#include "IComponentSerializer.h"

namespace Origo {

class ModelRendererComponentSerializer : public IComponentSerializer {
public:
	void serialize(Component* comp, ISerializer& s) const override;
	void deserialize(Component* comp, ISerializer& s) override;
};

}
