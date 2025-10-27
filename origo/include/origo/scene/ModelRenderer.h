#pragma once

#include "origo/assets/Model.h"
#include "origo/scene/Component.h"
namespace Origo {

class ModelRenderer : public Component {
public:
	ModelRenderer(const Ref<Entity>& entity, const Ref<Model>& model)
	    : Component(entity)
	    , m_Model(model) { };

	Ref<Model> GetModel() const { return m_Model; }

	std::string GetName() const override { return "ModelRenderer"; }

private:
	Ref<Model> m_Model;
};

}
