#include "origo/renderer/RenderCommand.h"

namespace Origo {
RenderCommand::RenderCommand(const RID& mesh, const RID& material, Transform* transform)
    : m_Mesh(mesh)
    , m_Material(material)
    , m_Transform(transform) {
}

RID RenderCommand::GetMaterial() const {
	return m_Material;
}

RID RenderCommand::GetMesh() const {
	return m_Mesh;
}

Transform* RenderCommand::GetTransform() const {
	return m_Transform;
}

}
