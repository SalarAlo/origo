#pragma once

#include "origo/assets/Model.h"
#include "origo/scene/Component.h"
namespace Origo {

class ModelRenderer : public Component {
public:
	ModelRenderer(Entity* entity, Model* model)
	    : Component(entity)
	    , m_Model(model) { };

	Model* GetModel() const { return m_Model; }

	std::string GetName() const override { return "ModelRenderer"; }

private:
	Model* m_Model;
};

}
