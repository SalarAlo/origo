#include "origo/components/particle_system/ParticleSystemComponent.h"

#include "origo/components/NativeComponentRegistry.h"

#include "origo/components/serialization/ParticleSystemComponentSerializer.h"

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::ParticleSystemComponent, Origo::ParticleSystemComponentSerializer);
