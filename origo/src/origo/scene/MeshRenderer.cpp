#include "origo/scene/MeshRenderer.h"
#include "origo/scene/NativeComponentRegistry.h"

namespace Origo {

MeshRenderer::MeshRenderer(AssetHandle material, AssetHandle mesh)
    : m_Material(material)
    , m_Mesh(mesh) {
}

ORIGO_REGISTER_NATIVE_COMPONENT(MeshRenderer)

}
