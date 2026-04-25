#include "MeshRendererComponent.h"

#include "NativeComponentRegistry.h"

#include "origo/components/serialization/MeshRendererComponentSerializer.h"

namespace Origo {

ORIGO_REGISTER_NATIVE_COMPONENT(MeshRendererComponent, MeshRendererComponentSerializer)

MeshRendererComponent::MeshRendererComponent(OptionalAssetHandle material, OptionalAssetHandle mesh)
    : MaterialHandle(material)
    , MeshHandle(mesh) {
}

}
