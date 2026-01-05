#pragma once

#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace Origo {

class ModelRenderer : public Component {
public:
	ModelRenderer(const AssetHandle& model)
	    : m_Model(model) { };

	AssetHandle GetModel() const { return m_Model; }
	void SetModel(const AssetHandle& model) { m_Model = model; }

	std::string GetName() const override { return "ModelRenderer"; }

private:
	AssetHandle m_Model;
};

}
