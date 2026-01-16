#pragma once

#include "origo/assets/AssetManager.h"
#include "Component.h"

namespace Origo {

class ModelRendererComponent : public Component {
public:
	ModelRendererComponent() = default;
	ModelRendererComponent(const AssetHandle& model)
	    : m_Model(model) { };

	OptionalAssetHandle GetModel() const { return m_Model; }
	void SetModel(const AssetHandle& model) { m_Model = model; }

	std::string GetComponentName() const override { return "ModelRenderer"; }

private:
	OptionalAssetHandle m_Model {};
};

}
