#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
MeshRenderer::MeshRenderer(Entity* entity, RID material, RID mesh)
    : Component(entity)
    , m_Material(material)
    , m_Mesh(mesh) {
}

RID MeshRenderer::GetMesh() {
	return m_Mesh;
}

RID MeshRenderer::GetMaterial() {
	return m_Material;
}

}
