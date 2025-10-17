#include "origo/scene/MeshRenderer.h"

namespace Origo {
MeshRenderer::MeshRenderer(const Ref<Material>& material, const Ref<Mesh>& mesh, Entity& entity)
    : Component(entity)
    , m_Material(material)
    , m_Mesh(mesh) {
}

Ref<Mesh> MeshRenderer::GetMesh() {
	return m_Mesh;
}

Ref<Material> MeshRenderer::GetMaterial() {
	return m_Material;
}

}
