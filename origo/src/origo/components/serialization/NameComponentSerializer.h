#pragma once

#include "IComponentSerializer.h"

namespace Origo {

class NameComponentSerializer : public IComponentSerializer {
public:
	void Serialize(Component* comp, ISerializer& s) const override;
	void Deserialize(Component* comp, ISerializer& s) override;
};

}
