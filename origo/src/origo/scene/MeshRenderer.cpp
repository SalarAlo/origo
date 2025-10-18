#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Component.h"

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

COMP_SERI_NAME_DEF(MeshRenderer)

nlohmann::json MeshRenderer::Serialize() const {
	using nlohmann::json;
	json serializedMeshRenderer;

	return serializedMeshRenderer;
}

void MeshRenderer::Deserialize(const nlohmann::json& j) {
}
}
