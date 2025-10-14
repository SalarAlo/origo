#pragma once

#include "origo/renderer/Material.h"
#include "origo/renderer/Mesh.h"
#include "origo/scene/Component.h"

#include "origo/scene/Entity.hpp"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(Ref<Material> material, Ref<Mesh> mesh, Entity& entity);

	Ref<Mesh> GetMesh();
	Ref<Material> GetMaterial();

private:
	Ref<Material> m_Material {};
	Ref<Mesh> m_Mesh {};
};

}
