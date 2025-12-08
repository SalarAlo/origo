#pragma once

#include "origo/core/UUID.h"
#include "origo/scene/Component.h"

#include "origo/scene/Entity.hpp"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(Entity* entity);
	MeshRenderer(Entity* entity, UUID material, UUID mesh);

	std::string GetName() const override { return "MeshRenderer"; }

	UUID GetMesh() const { return m_Mesh; }
	UUID GetMaterial() const { return m_Material; }

	void SetMesh(const UUID& uuid) { m_Mesh = uuid; }
	void SetMaterial(const UUID& uuid) { m_Mesh = uuid; }

private:
	UUID m_Material { UUID::Bad() };
	UUID m_Mesh { UUID::Bad() };
};

}
