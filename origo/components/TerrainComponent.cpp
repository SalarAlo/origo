#include "TerrainComponent.h"

#include "origo/components/NativeComponentRegistry.h"

#include "origo/components/serialization/TerrainComponentSerializer.h"

namespace Origo {

ORIGO_REGISTER_NATIVE_COMPONENT(TerrainComponent, TerrainComponentSerializer);

}
