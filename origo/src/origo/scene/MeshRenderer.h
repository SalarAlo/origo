#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(AssetHandle material = {}, AssetHandle mesh = {});

	std::string GetName() const override { return "MeshRenderer"; }

	AssetHandle GetMesh() const { return m_Mesh; }
	AssetHandle GetMaterial() const { return m_Material; }

	void SetMesh(const AssetHandle& handle) { m_Mesh = handle; }
	void SetMaterial(const AssetHandle& handle) { m_Material = handle; }

private:
	AssetHandle m_Material {};
	AssetHandle m_Mesh {};
};

}
