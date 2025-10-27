#pragma once

#include "origo/assets/Material.h"
#include "origo/assets/Mesh.h"
#include "origo/scene/Component.h"

#include "origo/scene/Entity.hpp"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(Ref<Entity> entity, const Ref<Material>& material, const Ref<Mesh>& mesh);

	Ref<Mesh> GetMesh();
	Ref<Material> GetMaterial();

	std::string GetName() const override { return "MeshRenderer"; }

private:
	Ref<Material> m_Material {};
	Ref<Mesh> m_Mesh {};
};

}
