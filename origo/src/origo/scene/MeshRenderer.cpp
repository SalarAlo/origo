#include "origo/scene/MeshRenderer.h"

namespace Origo {
MeshRenderer::MeshRenderer(AssetHandle material, AssetHandle mesh)
    : m_Material(material)
    , m_Mesh(mesh) {
}

}
