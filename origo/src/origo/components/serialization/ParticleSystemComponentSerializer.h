#pragma once
#include "IComponentSerializer.h"

namespace Origo {

class ParticleSystemComponentSerializer : public IComponentSerializer {
public:
	void Serialize(Component* comp, ISerializer& s) const override;
	void Deserialize(Component* comp, ISerializer& s) override;
};

}
