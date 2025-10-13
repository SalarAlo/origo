#include "origo/renderer/RenderCommand.h"

namespace Origo {
RenderCommand::RenderCommand(const Ref<Mesh>& mesh, const Ref<Material>& material, const Ref<Transform>& transform)
    : m_Mesh(mesh)
    , m_Material(material)
    , m_Transform(transform) {
}

Ref<Material> RenderCommand::GetMaterial() const {
	return m_Material;
}

Ref<Mesh> RenderCommand::GetMesh() const {
	return m_Mesh;
}

Ref<Transform> RenderCommand::GetTransform() const {
	return m_Transform;
}

}
