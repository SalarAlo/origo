#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
MeshRenderer::MeshRenderer(Entity* entity, Material* material, Mesh* mesh)
    : Component(entity)
    , m_Material(material)
    , m_Mesh(mesh) {
}

Mesh* MeshRenderer::GetMesh() {
	return m_Mesh;
}

Material* MeshRenderer::GetMaterial() {
	return m_Material;
}

}
