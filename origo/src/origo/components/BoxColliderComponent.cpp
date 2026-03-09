#include "origo/components/BoxColliderComponent.h"

#include "origo/components/NativeComponentRegistry.h"

#include "origo/components/serialization/BoxColliderComponentSerializer.h"

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::BoxColliderComponent, Origo::BoxColliderComponentSerializer);
