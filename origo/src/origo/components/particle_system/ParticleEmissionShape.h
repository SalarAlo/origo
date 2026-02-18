#pragma once

#include <variant>

#include "origo/components/particle_system/BoxEmissionShape.h"
#include "origo/components/particle_system/PointEmissionShape.h"
#include "origo/components/particle_system/SphereEmissionShape.h"

namespace Origo {

using ParticleEmissionShape = std::variant<PointEmissionShape, SphereEmissionShape, BoxEmissionShape>;

}
