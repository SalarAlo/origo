#include "ModelRenderer.h"

#include "NativeComponentRegistry.h"

#include "origo/components/serialization/MeshRendererComponentSerializer.h"
#include "origo/components/serialization/ModelRendererComponentSerializer.h"

ORIGO_REGISTER_NATIVE_COMPONENT(Origo::ModelRendererComponent, Origo::MeshRendererComponentSerializer);
