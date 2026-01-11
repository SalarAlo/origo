#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace Origo {

class ModelRenderer : public Component {
public:
	ModelRenderer() = default;
	ModelRenderer(const AssetHandle& model)
	    : m_Model(model) { };

	OptionalAssetHandle GetModel() const { return m_Model; }
	void SetModel(const AssetHandle& model) { m_Model = model; }

	std::string GetComponentName() const override { return "ModelRenderer"; }

	void Serialize(ISerializer& backend) const override;
	void Deserialize(ISerializer& backend) override;

private:
	OptionalAssetHandle m_Model {};
};

}
