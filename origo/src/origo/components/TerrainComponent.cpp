#include "origo/components/TerrainComponent.h"

#include "origo/components/NativeComponentRegistry.h"

#include "origo/components/serialization/TerrainComponentSerializer.h"

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::TerrainComponent, Origo::TerrainComponentSerializer)
