#include "origo/renderer/Renderable.h"

namespace Origo {
Renderable::Renderable(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material)
    : m_Mesh(mesh)
    , m_Material(material) {
}

std::shared_ptr<Material> Renderable::GetMaterial() const {
	return m_Material;
}

std::shared_ptr<Mesh> Renderable::GetMesh() const {
	return m_Mesh;
}

}
