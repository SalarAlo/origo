#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

#include "origo/scene/Entity.hpp"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(Entity* entity);
	MeshRenderer(Entity* entity, AssetHandle material, AssetHandle mesh);

	std::string GetName() const override { return "MeshRenderer"; }

	AssetHandle GetMesh() const { return m_Mesh; }
	AssetHandle GetMaterial() const { return m_Material; }

	void SetMesh(const AssetHandle& uuid) { m_Mesh = uuid; }
	void SetMaterial(const AssetHandle& uuid) { m_Mesh = uuid; }

private:
	AssetHandle m_Material {};
	AssetHandle m_Mesh {};
};

}
