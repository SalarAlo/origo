#include "origo/renderer/RenderCommand.h"

namespace Origo {
RenderCommand::RenderCommand(const UUID& mesh, const UUID& material, Transform* transform)
    : m_Mesh(mesh)
    , m_Material(material)
    , m_Transform(transform) {
}

UUID RenderCommand::GetMaterial() const {
	return m_Material;
}

UUID RenderCommand::GetMesh() const {
	return m_Mesh;
}

Transform* RenderCommand::GetTransform() const {
	return m_Transform;
}

}
