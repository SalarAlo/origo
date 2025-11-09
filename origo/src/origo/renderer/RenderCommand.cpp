#include "origo/renderer/RenderCommand.h"

namespace Origo {
RenderCommand::RenderCommand(Mesh* mesh, Material* material, Transform* transform)
    : m_Mesh(mesh)
    , m_Material(material)
    , m_Transform(transform) {
}

Material* RenderCommand::GetMaterial() const {
	return m_Material;
}

Mesh* RenderCommand::GetMesh() const {
	return m_Mesh;
}

Transform* RenderCommand::GetTransform() const {
	return m_Transform;
}

}
