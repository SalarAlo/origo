#include "origo/components/RigidbodyComponent.h"

#include "origo/components/NativeComponentRegistry.h"

#include "origo/components/serialization/RigidbodyComponentSerializer.h"

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::RigidbodyComponent, Origo::RigidbodyComponentSerializer);
