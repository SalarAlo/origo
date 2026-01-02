#pragma once

#include "origo/assets/AssetFactory.h"
#include "origo/assets/AssetManagerFast.h"
#include "origo/scene/Component.h"

namespace OrigoEditor {

class EditorOutline : public Origo::Component {
public:
	bool ShouldOutline = false;

public:
	EditorOutline() { }

	std::string GetName() const override { return "EditorSelection"; }

	static Origo::AssetHandle GetOutlineMaterial() {
		using namespace Origo;

		static bool s_Initialised { false };
		if (!s_Initialised) {
			auto shader = AssetFactory::CreateAsset<Shader>("Outline Shader");
			AssetManagerFast::GetInstance().Get<Shader>(shader)->SetSource(MakeScope<ShaderSourceFile>("resources/shaders/outline.glsl"));

			m_OutlineMaterial = AssetFactory::CreateAsset<Material2D>("Outline Shader", shader);

			s_Initialised = true;
		}

		return m_OutlineMaterial;
	}

private:
	inline static Origo::AssetHandle m_OutlineMaterial {};
};

}
