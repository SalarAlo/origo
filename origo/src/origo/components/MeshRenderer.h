#pragma once

#include "origo/assets/AssetManager.h"
#include "Component.h"

namespace Origo {

class MeshRendererComponent : public Component {
public:
	MeshRendererComponent(OptionalAssetHandle material = {}, OptionalAssetHandle mesh = {});

	std::string GetComponentName() const override { return "MeshRenderer"; }

	OptionalAssetHandle GetMesh() const { return m_Mesh; }
	OptionalAssetHandle GetMaterial() const { return m_Material; }

	void SetMesh(const AssetHandle& handle) { m_Mesh = handle; }
	void SetMaterial(const AssetHandle& handle) { m_Material = handle; }

private:
	OptionalAssetHandle m_Material { std::nullopt };
	OptionalAssetHandle m_Mesh { std::nullopt };
	Vec3 m_Color {};
};

}
