#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
MeshRenderer::MeshRenderer(Ref<Entity> entity, const Ref<Material>& material, const Ref<Mesh>& mesh)
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
