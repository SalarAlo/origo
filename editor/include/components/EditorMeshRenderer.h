#pragma once

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"
#include "origo/scene/Entity.hpp"

namespace OrigoEditor {

class EditorMeshRenderer : public Origo::Component {

public:
	EditorMeshRenderer(Origo::Entity* entity)
	    : Component(entity) { }

	EditorMeshRenderer(Origo::Entity* entity, Origo::AssetHandle material, Origo::AssetHandle mesh)
	    : Component(entity)
	    , m_Material(material)
	    , m_Mesh(mesh) { }

	std::string GetName() const override { return "EditorMeshRenderer"; }

	Origo::AssetHandle GetMesh() const { return m_Mesh; }
	Origo::AssetHandle GetMaterial() const { return m_Material; }

	bool IsSelected = false;

	void SetMesh(const Origo::AssetHandle& mesh) { m_Mesh = mesh; }
	void SetMaterial(const Origo::AssetHandle& material) { m_Material = material; }
	static Origo::AssetHandle GetOutlineMaterial() {
		using namespace Origo;

		static bool s_Initialised { false };
		if (!s_Initialised) {
			auto shader = AssetFactory::CreateAsset<Shader>("Outline Shader");
			AssetManagerFast::GetInstance().Get<Shader>(shader)->SetSource(MakeScope<ShaderSourceFile>("resources/shaders/outline.glsl"));

			m_OutlineMaterial = AssetFactory::CreateAsset<Material>("Outline Shader", shader);

			s_Initialised = true;
		}

		return m_OutlineMaterial;
	}

private:
	inline static Origo::AssetHandle m_OutlineMaterial {};
	Origo::AssetHandle m_Material {};
	Origo::AssetHandle m_Mesh {};
};

}
