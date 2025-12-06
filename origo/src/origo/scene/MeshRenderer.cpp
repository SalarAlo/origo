#include "origo/scene/MeshRenderer.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"

namespace Origo {
MeshRenderer::MeshRenderer(Entity* entity, UUID material, UUID mesh)
    : Component(entity)
    , m_Material(material)
    , m_Mesh(mesh) {
}

UUID MeshRenderer::GetMesh() {
	return m_Mesh;
}

UUID MeshRenderer::GetMaterial() {
	return m_Material;
}

}
