#include "origo/renderer/RenderCommand.h"

namespace Origo {
RenderCommand::RenderCommand(const AssetHandle& mesh, const AssetHandle& material, Transform* transform)
    : m_Mesh(mesh)
    , m_Material(material)
    , m_Transform(transform) {
}

AssetHandle RenderCommand::GetMaterial() const {
	return m_Material;
}

AssetHandle RenderCommand::GetMesh() const {
	return m_Mesh;
}

Transform* RenderCommand::GetTransform() const {
	return m_Transform;
}

}
