#pragma once

#include "origo/assets/Material.h"
#include "origo/assets/Mesh.h"
#include "origo/scene/Component.h"

#include "origo/scene/Entity.hpp"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(Entity* entity, Material* material, Mesh* mesh);

	Mesh* GetMesh();
	Material* GetMaterial();

	std::string GetName() const override { return "MeshRenderer"; }

private:
	Material* m_Material {};
	Mesh* m_Mesh {};
};

}
