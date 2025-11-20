#pragma once

#include "origo/scene/Component.h"

#include "origo/scene/Entity.hpp"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(Entity* entity, RID material, RID mesh);

	RID GetMesh();
	RID GetMaterial();

	std::string GetName() const override { return "MeshRenderer"; }

private:
	RID m_Material {};
	RID m_Mesh {};
};

}
