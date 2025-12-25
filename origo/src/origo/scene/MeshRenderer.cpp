#include "origo/scene/MeshRenderer.h"
#include "origo/scene/ComponentRegistry.h"

namespace Origo {

MeshRenderer::MeshRenderer(AssetHandle material, AssetHandle mesh)
    : m_Material(material)
    , m_Mesh(mesh) {
}

ORIGO_REGISTER_COMPONENT(MeshRenderer)

}
