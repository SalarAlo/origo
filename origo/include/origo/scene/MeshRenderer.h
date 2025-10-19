#pragma once

#include "origo/renderer/Material.h"
#include "origo/renderer/Mesh.h"
#include "origo/scene/Component.h"

#include "origo/scene/Entity.hpp"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(const Ref<Material>& material, const Ref<Mesh>& mesh, Ref<Entity> entity);

	Ref<Mesh> GetMesh();
	Ref<Material> GetMaterial();

	std::string GetName() const override { return "MeshRenderer"; }

private:
	Ref<Material> m_Material {};
	Ref<Mesh> m_Mesh {};
};

}
