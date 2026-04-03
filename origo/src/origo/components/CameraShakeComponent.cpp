#include "origo/components/CameraShakeComponent.h"

#include "origo/components/NativeComponentRegistry.h"

#include "origo/components/serialization/CameraShakeComponentSerializer.h"

namespace Origo {
ORIGO_REGISTER_NATIVE_COMPONENT(CameraShakeComponent, CameraShakeComponentSerializer)
}
