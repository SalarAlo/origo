#pragma once

#include "origo/assets/Material.h"
#include "origo/assets/Model.h"
#include "origo/scene/Component.h"
namespace Origo {

class ModelRenderer : public Component {
public:
	ModelRenderer(const Ref<Model>& model, const Ref<Material>& material, const Ref<Entity>& entity)
	    : Component(entity)
	    , m_Model(model)
	    , m_Material(material) { };

	Ref<Model> GetModel() const { return m_Model; }
	Ref<Material> GetMaterial() const { return m_Material; }

	std::string GetName() const override { return "ModelRenderer"; }

private:
	Ref<Model> m_Model;
	Ref<Material> m_Material;
};

}
