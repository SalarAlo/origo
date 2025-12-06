#pragma once

#include "origo/core/UUID.h"
#include "origo/scene/Component.h"

#include "origo/scene/Entity.hpp"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(Entity* entity, UUID material, UUID mesh);

	UUID GetMesh();
	UUID GetMaterial();

	std::string GetName() const override { return "MeshRenderer"; }

private:
	UUID m_Material {};
	UUID m_Mesh {};
};

}
