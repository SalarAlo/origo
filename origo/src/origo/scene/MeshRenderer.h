#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace Origo {

class MeshRenderer : public Component {
public:
	MeshRenderer(OptionalAssetHandle material = {}, OptionalAssetHandle mesh = {});

	std::string GetComponentName() const override { return "MeshRenderer"; }

	OptionalAssetHandle GetMesh() const { return m_Mesh; }
	OptionalAssetHandle GetMaterial() const { return m_Material; }

	void SetMesh(const AssetHandle& handle) { m_Mesh = handle; }
	void SetMaterial(const AssetHandle& handle) { m_Material = handle; }

	void Serialize(ISerializer& backend) const override;
	void Deserialize(ISerializer& backend) override;

private:
	OptionalAssetHandle m_Material { std::nullopt };
	OptionalAssetHandle m_Mesh { std::nullopt };
};

}
