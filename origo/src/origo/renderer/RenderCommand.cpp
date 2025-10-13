#include "origo/renderer/RenderCommand.h"

namespace Origo {
RenderCommand::RenderCommand(Ref<Mesh> mesh, Ref<Material> material)
    : m_Mesh(mesh)
    , m_Material(material) {
}

Ref<Material> RenderCommand::GetMaterial() const {
	return m_Material;
}

Ref<Mesh> RenderCommand::GetMesh() const {
	return m_Mesh;
}

}
