#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
MeshRenderer::MeshRenderer(Entity* entity, AssetHandle material, AssetHandle mesh)
    : Component(entity)
    , m_Material(material)
    , m_Mesh(mesh) {
}

}
